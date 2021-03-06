/**************************************************************************

  Copyright 2015-2019 Rafael Dellà Bort. silderan (at) gmail (dot) com

  This file is part of BasicSNMP

  BasicSNMP is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as
  published by the Free Software Foundation, either version 3 of
  the License, or (at your option) any later version.

  BasicSNMP is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  and GNU Lesser General Public License. along with BasicSNMP.
  If not, see <http://www.gnu.org/licenses/>.

**************************************************************************/

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "lib/snmplib.h"
#include "lib/asn1types.h"

#include <QStyledItemDelegate>

using namespace SNMP;

const QList<ASN1DataType> &MainWindow::asn1Types()
{
	static QList<ASN1DataType> setRequestTypes = QList<ASN1DataType>()
																<< ASN1TYPE_INTEGER
																<< ASN1TYPE_Counter
																<< ASN1TYPE_Gauge32
																<< ASN1TYPE_Unsigned64
																<< ASN1TYPE_BOOLEAN
																<< ASN1TYPE_BITSTRING
																<< ASN1TYPE_OCTETSTRING
																<< ASN1TYPE_NULL
																<< ASN1TYPE_OBJECTID
																<< ASN1TYPE_OBJECTDESC
																<< ASN1TYPE_EXTERNAL
																<< ASN1TYPE_REAL
																<< ASN1TYPE_ENUMERATED
																<< ASN1TYPE_UTCTime;
	return setRequestTypes;
}

#define TABLE_REQUEST_ID	10000

class QStatusColumnComboBoxDelegate : public QStyledItemDelegate
{
	std::function<SMIVersion()> mGetSMIVer;

public:
	QStatusColumnComboBoxDelegate(std::function<SMIVersion()> getSMIVer)
		: mGetSMIVer(getSMIVer)
	{

	}
	QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
	{
		Q_UNUSED(option);
		Q_UNUSED(index);

		switch( mGetSMIVer() )
		{
		case SMIVersion::SMIv1:
		{
			QComboBox *cb = new QComboBox(parent);
			QMapIterator<StatusEntry, QString> it(SNMPConstants::entryStatusInfoMap());
			while( it.hasNext() )
			{
				it.next();
				cb->addItem( it.value(), it.key() );
			}

			return cb;
		}
		case SMIVersion::SMIv2:
		{
			QComboBox *cb = new QComboBox(parent);
			QMapIterator<RowStatus, QString> it(SNMPConstants::rowStatusInfoMap());
			while( it.hasNext() )
			{
				it.next();
				cb->addItem( it.value(), it.key() );
			}

			return cb;
		}
		default:
			break;
		}
		return Q_NULLPTR;
	}
	void setEditorData(QWidget *editor, const QModelIndex &index) const
	{
		int sr = index.model()->data(index, Qt::UserRole).toInt();

		QComboBox *cb = static_cast<QComboBox*>(editor);
		for( int i = 0; i < cb->count(); ++i )
		{
			if( cb->itemData(i).toInt() == sr )
			{
				cb->setCurrentIndex(i);
				break;
			}
		}
	}
	void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
	{
		QComboBox *cb = static_cast<QComboBox*>(editor);

		// Need to get data before setting it on model because when setting them, setEditorData is called.
		QString txt = cb->itemText(cb->currentIndex());
		int data = cb->itemData(cb->currentIndex()).toInt();
		model->setData( index, txt );
		model->setData( index, data, Qt::UserRole );
	}
	void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
	{
		Q_UNUSED(index);
		editor->setGeometry(option.rect);
	}
};

QComboBox *statusRowComboBoxDelegate()
{
	return new QComboBox();
}

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
	, mRequestID(1)
{
	ui->setupUi(this);

	ui->version->addItem("SNMPv1", 0);
	ui->version->addItem("SNMPv2c", 1);
	ui->comunity->addItems(QStringList() << "public" << "private");

	configData.load();
	// "1.3.6.1.2.1.1.5.0");//".1.3.1.1.2.1.31.1.1.1.0.3.0.0");//".1.3.6.1.2.1.31.1.1.1.6.3");//1.3.6.1.2.1.1.5.0");
	ui->OIDLineEdit->setText( configData.oid().isEmpty() ? "1.3" : configData.oid() );
	ui->agentIP->setText( configData.agentIP().isEmpty() ? "192.168.1.1" : configData.agentIP() );
	ui->agentPort->setValue( configData.agentPort() != 0 ? configData.agentPort() : 161);
	ui->trapPort->setValue( configData.trapPort() != 0 ? configData.trapPort() : 162 );
	ui->trapEnabled->setChecked( configData.trapEnabled() );
	ui->version->setCurrentIndex( configData.snmpVersion() );
	ui->comunity->setCurrentText( configData.comunity().isEmpty() ? "public" : configData.comunity() );
	ui->user->setText( configData.userName() );
	ui->pass->setText( configData.userPass() );

	snmpConn.setIncludeRawData(true);

	setupValueTypeComboBox(ui->valueType, configData.valueType());
	setupValueTypeComboBox(ui->columnValueType, ASN1TYPE_INTEGER);

	setupSNMPTableTab();

	connect( &snmpConn, &SNMPConn::dataReceived, this, &MainWindow::onSNMPDataReceived );
	connect( &snmpConn, &SNMPConn::tableCellReceived, this, &MainWindow::onTableCellReceived );
	connect( &snmpConn, &SNMPConn::tableReceived, this, &MainWindow::onTableReceived );

	connect( ui->keyCount, SIGNAL(valueChanged(int)), this, SLOT(onKeyColumnCountChanged(int)) );
	connect( ui->columnCount, SIGNAL(valueChanged(int)), this, SLOT(onRegularColumnCountChanged(int)) );
	connect( ui->readOnly, &QCheckBox::stateChanged, this, &MainWindow::onReadOnlyChanged );
	connect( ui->columnValueType, SIGNAL(currentIndexChanged(int)), this, SLOT(onColumnValueTypeChanged(int)) );
	connect( ui->smiVersion, SIGNAL(currentIndexChanged(int)), this, SLOT(onSMIVersionChanged(int)) );
	connect( ui->rows, SIGNAL(valueChanged(int)), this, SLOT(onRowsChanged(int)) );

	snmpConn.setTrapHost(162);
}

MainWindow::~MainWindow()
{
	configData.setOID( ui->OIDLineEdit->text() );
	configData.setAgentIP( ui->agentIP->text() );
	configData.setAgentPort( static_cast<quint16>(ui->agentPort->value()) );
	configData.setTrapEnabled( ui->trapEnabled->isChecked() );
	configData.setTrapPort( static_cast<quint16>(ui->trapPort->value()) );
	configData.setSNMPVersion( static_cast<quint8>(ui->version->currentIndex()) );
	configData.setComunity( ui->comunity->currentText() );
	configData.setUserName( ui->user->text() );
	configData.setUserPass( ui->pass->text() );
	configData.setValueType( currentValueType(ui->valueType) );
	configData.setTableColumnsInfoList( mTableColumnInfoList );
	configData.setSMIVersion( mSMIVersion );
	configData.setBaseTableOID( ui->oidBase->text() );

	configData.save();
	delete ui;
}

ASN1DataType MainWindow::currentValueType(QComboBox *cb) const
{
	return valueType(cb, cb->currentIndex());
}

ASN1DataType MainWindow::valueType(QComboBox *cb, int index) const
{
	int value = cb->itemData(index, Qt::UserRole).value<int>();
	return static_cast<ASN1DataType>(value);
}

void MainWindow::selectValueType(QComboBox *cb, ASN1DataType valueType)
{
	for( int i = 0; i < cb->count(); ++i )
		if( this->valueType(cb, i) == valueType )
		{
			cb->setCurrentIndex(i);
			break;
		}
}

void MainWindow::setupValueTypeComboBox(QComboBox *cb, ASN1DataType selected)
{
	for( ASN1DataType dataType : asn1Types() )
	{
		int value = static_cast<int>(dataType);
		cb->addItem( SNMPConstants::asn1TypeName(dataType), value );
		Q_ASSERT( dataType == valueType(cb, cb->count()-1) );
		if( dataType == selected )
			cb->setCurrentIndex(cb->count()-1);
	}
}

void MainWindow::addReplyRow(const OID &oid, const QString &valueType, const QString &value, const QString &rawValue)
{
	int row = ui->replyTable->rowCount();
	QTableWidgetItem *item;

	ui->replyTable->insertRow(row);
	ui->replyTable->setItem(row, 0, item = new QTableWidgetItem(QString::fromStdString(oid.toStdString())) );
	item->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEnabled );
	ui->replyTable->setItem(row, 1, item = new QTableWidgetItem(QString("%1: %2").arg(valueType, value)) );
	item->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEnabled );
	item->setData(Qt::UserRole, value);
	ui->replyTable->setItem(row, 2, item = new QTableWidgetItem(rawValue) );
	item->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEnabled );
}

void MainWindow::addReplyRow(const PDUVarbind &varbind)
{
	addReplyRow( varbind.oid(),
				 SNMPConstants::asn1TypeName(varbind.asn1Variable().type()),
				 SNMPConstants::asn1PrintableValue(varbind.asn1Variable()),
				 SNMPConstants::printableRawData(varbind) );
}

void MainWindow::onSNMPDataReceived(const Encoder &snmp)
{
	if( snmp.requestID() > TABLE_REQUEST_ID )
	{
		updateSnmpTableCell( TableInfo(ui->oidBase->text().toStdString().data(), snmp) );

		if( mGetColumnsOidList.count() )
			getColumns_RequestFirst();
		return;
	}
	ui->replyTable->setRowCount(0);

	if( !snmp.varbindList().count() )
		ui->statusBar->showMessage( tr("Ningun dato recibido. Error: %1").arg(SNMPConstants::printableErrorCode(snmp)) );
	else
	{
		for( const PDUVarbind &varbind : snmp.varbindList() )
			addReplyRow( varbind );
		ui->statusBar->showMessage( SNMPConstants::printableErrorCode(snmp) );
	}
}

void MainWindow::on_sendGetRequest_clicked()
{
	snmpConn.setAgentHost( ui->agentIP->text(), static_cast<quint16>(ui->agentPort->value()) );
	snmpConn.sendGetRequest( ui->version->currentData(Qt::UserRole).toInt(),
							 ui->OIDLineEdit->text(),
							 ui->comunity->currentText(),
							 ++mRequestID );
}

void MainWindow::on_sendGetNextRequest_clicked()
{
	snmpConn.setAgentHost( ui->agentIP->text(), static_cast<quint16>(ui->agentPort->value()) );
	snmpConn.sendGetNextRequest( ui->version->currentData(Qt::UserRole).toInt(), ui->OIDLineEdit->text(), ui->comunity->currentText(), ++mRequestID );
}

void MainWindow::on_sendSetRequest_clicked()
{
	ASN1Variable asn1Var;
	switch( ui->valueType->currentData().toInt() )
	{
	case ASN1TYPE_BOOLEAN:		asn1Var.setBoolean( ui->value->text()=="true" );			break;
	case ASN1TYPE_INTEGER:		asn1Var.setInteger( ui->value->text().toInt() );			break;
	case ASN1TYPE_Counter:		asn1Var.setCounter( ui->value->text().toUInt() );			break;
	case ASN1TYPE_Gauge32:		asn1Var.setGauge32( ui->value->text().toUInt() );			break;
	case ASN1TYPE_Unsigned64:	asn1Var.setUnsigned64( ui->value->text().toULongLong() );	break;
	case ASN1TYPE_OCTETSTRING:
		asn1Var.setOctetString( ui->value->text().toStdString() );
		break;
	default:
		ui->statusBar->showMessage( tr("Envio de datos tipo %1 no implementado aun")
									.arg( SNMPConstants::asn1TypeName( static_cast<ASN1DataType>(ui->valueType->currentData().toInt()))) );
		return;
	}
	snmpConn.setAgentHost( ui->agentIP->text(), static_cast<quint16>(ui->agentPort->value()) );
	snmpConn.sendSetRequest( ui->version->currentData(Qt::UserRole).toInt(),
							 ui->OIDLineEdit->text(),
							 ui->comunity->currentText(),
							 asn1Var,
							 ++mRequestID );
}

void MainWindow::on_replyTable_cellDoubleClicked(int row, int column)
{
	if( (column == 0) && ui->replyTable->item(row, 0) && !ui->replyTable->item(row, 0)->text().isEmpty() )
		ui->OIDLineEdit->setText( ui->replyTable->item(row, 0)->text() );
	else
	if( (column == 1) && ui->replyTable->item(row, 1) && ui->replyTable->item(row, 1)->data(Qt::UserRole).isValid() )
		ui->value->setText( ui->replyTable->item(row, 1)->data(Qt::UserRole).toString() );
}

void MainWindow::on_version_currentIndexChanged(int index)
{
	ui->sendGetBulkRequest->setEnabled(index != 0);
	ui->quantity->setEnabled(index != 0);
	ui->user->setEnabled(index == 2);
	ui->pass->setEnabled(index == 2);
}

void MainWindow::on_trapEnabled_stateChanged(int checked)
{
	// ToDo: Trap Not yet programed in API
	ui->trapPort->setEnabled(checked);
}

void MainWindow::updateHeaderLabel(int col)
{
	Q_UNUSED(col);
	ui->snmpTable->setHorizontalHeaderLabels(mTableColumnInfoList.descriptions());
}

void MainWindow::updateLocalIndexComboBox()
{
	int oldLocalIndex = ui->columnLocalIndex->currentIndex();
	int oldStatusColumn = ui->statusColumn->currentData().toInt();

	ui->columnLocalIndex->blockSignals(true);
	ui->statusColumn->blockSignals(true);
	ui->columnLocalIndex->clear();
	ui->statusColumn->clear();

	ui->statusColumn->addItem( "No status column", -1 );
	QString text;
	for( int col = 0; col < mTableColumnInfoList.count(); ++col )
	{
		if( mTableColumnInfoList.isKeyColumn(col) )
			text = QString("%1 (Key %2)").arg(col).arg(col + 1);
		else
			text = QString("%1 (Column %2)").arg(col).arg(col - mTableColumnInfoList.keyColumnCount() + 1);
		ui->columnLocalIndex->addItem( text, col );
		if( !mTableColumnInfoList.isKeyColumn(col) )
			ui->statusColumn->addItem( text, col );
	}

	ui->columnLocalIndex->blockSignals( false );
	ui->statusColumn->blockSignals( false );

	ui->columnLocalIndex->setCurrentIndex( (oldLocalIndex < ui->columnLocalIndex->count()) ? oldLocalIndex : (ui->columnLocalIndex->count()-1) );
	if( oldStatusColumn != -1 )
	{
		for( int i = 0; i < ui->statusColumn->count(); ++ i )
		{
			if( ui->statusColumn->itemData(i).toInt() == oldStatusColumn )
			{
				ui->statusColumn->setCurrentIndex(i);
				break;
			}
		}
	}
}

void MainWindow::onKeyColumnCountChanged(int count)
{
	int currentCount = mTableColumnInfoList.keyColumnCount();

	while( currentCount < count )
	{
		ui->snmpTable->insertColumn( currentCount );
		mTableColumnInfoList.appendKeyColumn( TableColumnInfo(ASN1TYPE_INTEGER, true, "OID Key") );
		++currentCount;
	}
	while( currentCount > count )
	{
		ui->snmpTable->removeColumn(currentCount-1);
		mTableColumnInfoList.removeKeyColumn(currentCount-1);
		--currentCount;
	}
	updateHeaderLabel(-1);
	updateLocalIndexComboBox();
}

void MainWindow::onRegularColumnCountChanged(int regularColumnCount)
{
	int newTotal = mTableColumnInfoList.keyColumnCount() + regularColumnCount;

	while( newTotal > ui->snmpTable->columnCount() )
	{
		ui->snmpTable->insertColumn( ui->snmpTable->columnCount() );
		mTableColumnInfoList.appendRegularColumn( TableColumnInfo(ASN1TYPE_INTEGER, true, "Column") );
	}
	while( newTotal < ui->snmpTable->columnCount() )
	{
		ui->snmpTable->removeColumn( ui->snmpTable->columnCount()-1 );
		mTableColumnInfoList.removeRegularColumn( mTableColumnInfoList.regularColumnCount()-1 );
	}
	updateHeaderLabel(-1);
	updateLocalIndexComboBox();
}

void MainWindow::onReadOnlyChanged(int newState)
{
	Q_UNUSED(newState);
	// TODO: Switch al column editability.
	int index = ui->columnLocalIndex->currentIndex();
	if( index != -1 )
	{
		mTableColumnInfoList[index].readOnly = (newState == Qt::Checked);
	}
}

void MainWindow::setupSNMPTableTab()
{
	mTableColumnInfoList = configData.tableColumnsInfoList();
	mSMIVersion = configData.smiVersion();
	ui->oidBase->setText( configData.baseTableOID() );

	ui->snmpTable->setColumnCount( mTableColumnInfoList.count() );
	ui->keyCount->setValue( mTableColumnInfoList.keyColumnCount() );
	ui->columnCount->setValue( mTableColumnInfoList.count() - ui->keyCount->value() );

	ui->smiVersion->addItem( "SMIv1 (EntryStatus)", static_cast<int>(SMIVersion::SMIv1) );
	ui->smiVersion->addItem( "SMIv2 (RowStatus)", static_cast<int>(SMIVersion::SMIv2) );
	ui->smiVersion->setCurrentIndex( mSMIVersion - SMIVersion::SMIv1 );

	updateHeaderLabel( -1 );
	updateLocalIndexComboBox();
}

void MainWindow::on_columnLocalIndex_currentIndexChanged(int index)
{
	if( index != -1 )
	{
		const TableColumnInfo &colInfo = mTableColumnInfoList[index];
		selectValueType( ui->columnValueType, colInfo.valueType );
		ui->columnAlias->setText( colInfo.name );
		ui->readOnly->setChecked( mTableColumnInfoList.isReadOnlycolumn(index) );
		ui->readOnly->setDisabled( mTableColumnInfoList.isKeyColumn(index) );
	}
}

void MainWindow::on_columnAlias_textChanged(const QString &text)
{
	int index = ui->columnLocalIndex->currentIndex();
	if( index != -1 )
	{
		mTableColumnInfoList[index].name = text;
		updateHeaderLabel(index);
	}
}

void MainWindow::onColumnValueTypeChanged(int valueTypeIndex)
{
	int index = ui->columnLocalIndex->currentIndex();
	if( index != -1 )
	{
		mTableColumnInfoList[index].valueType = valueType(ui->valueType, valueTypeIndex);
		updateHeaderLabel(index);
	}
}

void MainWindow::onRowsChanged(int rows)
{
	while( rows > ui->snmpTable->rowCount() )
		ui->snmpTable->insertRow(ui->snmpTable->rowCount());
	while( rows < ui->snmpTable->rowCount() )
		ui->snmpTable->removeRow(ui->snmpTable->rowCount()-1);
}

void MainWindow::onSMIVersionChanged(int index)
{
	switch( mSMIVersion = static_cast<SMIVersion>( ui->smiVersion->itemData(index).toInt()) )
	{
	case SMIVersion::SMIUnd:
		ui->statusColumn->setEnabled(false);
		ui->statusColumnLabel->setText( tr("Choose SMIVersion") );
		break;
	case SMIVersion::SMIv1:
		ui->statusColumn->setEnabled(true);
		ui->statusColumnLabel->setText( tr("Column for EntryStatus") );
		break;
	case SMIVersion::SMIv2:
		ui->statusColumn->setEnabled(true);
		ui->statusColumnLabel->setText( tr("Column for StatusRow") );
		break;
	}
	updateHeaderLabel(-1);
	updateStatusColumnValues(-1);
}

void MainWindow::on_getColumnsStart_clicked()
{
	if( snmpConn.isDiscoveringTable(TABLE_REQUEST_ID) )
		ui->statusBar->showMessage( tr("Aun se está descubriendo la tabla. Cancela o espera a que termine el proceso.") );
	else
	if( !ui->oidBase->text().length() )
		ui->statusBar->showMessage( tr("Falta el OID base de la tabla") );
	else
	{
		int initialColum = ui->getColumnsFirstColumn->value();
		int finalColumn = ui->getColumnsLastColumn->value();

		if( finalColumn > ui->columnCount->value() )
		{
			finalColumn = ui->columnCount->value();
			ui->getColumnsLastColumn->setValue(finalColumn);
		}
		if( initialColum > ui->columnCount->value() )
		{
			initialColum = ui->columnCount->value();
			ui->getColumnsFirstColumn->setValue(initialColum);
		}
		if( initialColum > finalColumn )
		{
			finalColumn = initialColum;
			ui->getColumnsLastColumn->setValue(finalColumn);
		}
		while( initialColum <= finalColumn )
		{
			QString oidBase = ui->oidBase->text();
			QString oidKeys;
			if( !oidBase.endsWith('.') )
				oidBase.append('.');

			// Go though all rows of all columns to clean text and, also, get OID List
			for( int row = 0; row < ui->snmpTable->rowCount(); ++row )
			{
				oidKeys.clear();
				for( int col = 0; col < ui->keyCount->value(); col++ )
				{
					oidKeys.append( QString(".%1").arg(ui->snmpTable->item(row, col)->text()) );
					ui->snmpTable->item(row, ui->keyCount->value() + initialColum - 1)->setText("");
				}
				mGetColumnsOidList.append( QString("%1%2%3").arg(oidBase).arg(initialColum).arg(oidKeys) );
			}

			initialColum++;
		}
		if( ui->getColumnsRequestCount->value() < 1 )
			ui->getColumnsRequestCount->setValue(1);
		for( int i = 0; (i < ui->getColumnsRequestCount->value()) && (i < mGetColumnsOidList.count()); i++ )
			getColumns_RequestFirst();
	}
}

void MainWindow::getColumns_RequestFirst()
{
	if( mGetColumnsOidList.count() )
	{
		SNMP::Encoder snmp;
		snmp.setVersion( ui->version->currentData(Qt::UserRole).toInt() );
		snmp.setComunity( ui->comunity->currentText().toStdString() );
		snmp.setRequestType( ASN1TYPE_GetRequestPDU );

		snmp.setRequestID( TABLE_REQUEST_ID + mGetColumnsOidList.count() );
		snmp.setObjectIdentifier( mGetColumnsOidList.first().toLatin1().data() );
		mGetColumnsOidList.pop_front();
		snmpConn.sendRequest(snmp);
	}
}

void MainWindow::on_discoverTable_clicked()
{
	if( snmpConn.isDiscoveringTable(TABLE_REQUEST_ID) )
	{
		snmpConn.cancelDiscoverTable(TABLE_REQUEST_ID);
		onTableReceived(-1);
	}
	else
	if( !ui->oidBase->text().length() )
		ui->statusBar->showMessage( tr("Falta el OID base de la tabla") );
	else
	{
		ui->oidBase->setDisabled(true);
		ui->discoverTable->setText( tr("Cancel discovery") );
		ui->snmpTable->setRowCount(0);
		ui->keyCount->setValue(1);
		ui->columnCount->setValue(1);
		ui->statusColumn->setCurrentIndex(0);
		snmpConn.setAgentHost( ui->agentIP->text(), static_cast<quint16>(ui->agentPort->value()) );
		snmpConn.discoverTable( ui->version->currentData(Qt::UserRole).toInt(), ui->oidBase->text(), ui->comunity->currentText(), TABLE_REQUEST_ID );
	}
}

int MainWindow::rowOf(OID keys)
{
	int col, row;
	for( row = 0; row < ui->snmpTable->rowCount(); ++row )
	{
		for( col = 0; col < keys.count(); ++col )
		{
			if( !ui->snmpTable->item(row, col) || (keys[col] != ui->snmpTable->item(row, col)->text().toStdString()) )
				break;
		}
		// Matched
		if( col == keys.count() )
			break;
	}

	// If row is a new one...
	if( row == ui->snmpTable->rowCount() )
	{
		// By setting this value, row count will increase acordingly.
		ui->rows->setValue(row+1);

		// Put OID key values into widget table cells.
		for( col = 0; col < keys.count(); ++col )
			ui->snmpTable->setItem(row, col, new QTableWidgetItem( QString::fromStdString(keys[col].toStdString())) );
	}
	return row;
}

void MainWindow::updateStatusColumnValues(int col)
{
	if( (col == -1) && ((col = mTableColumnInfoList.statusColumnIndex()) == -1) )
		return;
	if( mTableColumnInfoList.isStatusColumn(col) )
	{
		for( int row = 0; row < ui->snmpTable->rowCount(); ++row )
		{
			if( !ui->snmpTable->item(row, col) )
				ui->snmpTable->setItem(row, col, new QTableWidgetItem());

			if( !ui->snmpTable->item(row, col)->data(Qt::UserRole).isValid() )
				ui->snmpTable->item(row, col)->setData( Qt::UserRole, ui->snmpTable->item(row, col)->text() );

			int statusRow = ui->snmpTable->item(row, col)->data(Qt::UserRole).toString().toInt();
			switch( mSMIVersion )
			{
			case SMIVersion::SMIv1:
				ui->snmpTable->item(row, col)->setText( SNMPConstants::entryStatusName( static_cast<StatusEntry>(statusRow)) );
				break;
			case SMIVersion::SMIv2:
				ui->snmpTable->item(row, col)->setText( SNMPConstants::statusRowName( static_cast<RowStatus>(statusRow)) );
				break;
			default:
				ui->snmpTable->item(row, col)->setText( "Choose SMIVersion" );
			}
		}
	}
	else
	{
		for( int row = 0; row < ui->snmpTable->rowCount(); ++row )
			if( ui->snmpTable->item(row, col) )
				ui->snmpTable->item(row, col)->setText(ui->snmpTable->item(row, col)->data(Qt::UserRole).toString());
	}
}

void MainWindow::updateSnmpTableCell(const TableInfo &tableInfo)
{
	if( tableInfo.keyIndexes.count() != ui->keyCount->value() )
	{
		ui->keyCount->setValue( static_cast<int>(tableInfo.keyIndexes.count()) );
	}

	// If incoming cell is for a new one column, let's modify the table widget to make room for it.
	if( tableInfo.column >= static_cast<UInt64>(ui->columnCount->value()) )
	{
		ui->columnCount->setValue( static_cast<int>(tableInfo.column) );
		mTableColumnInfoList[tableInfo.column + tableInfo.keyIndexes.count() - 1].valueType = tableInfo.varbind.asn1Variable().type();
		updateHeaderLabel(-1);
	}

	int row = rowOf( tableInfo.keyIndexes );
	QTableWidgetItem *item = new QTableWidgetItem( SNMPConstants::asn1PrintableValue(tableInfo.varbind.asn1Variable()) );
	item->setToolTip( SNMPConstants::printableRawData(tableInfo.varbind) );

	ui->snmpTable->setItem( row, tableInfo.column + tableInfo.keyIndexes.count() - 1, item );
}

void MainWindow::onTableCellReceived(const Encoder &snmp)
{
	ui->statusBar->showMessage( SNMPConstants::printableErrorCode(snmp) );

	updateSnmpTableCell( TableInfo(snmpConn.tableBaseOID(TABLE_REQUEST_ID), snmp) );
}

void MainWindow::onTableReceived(int requestID)
{
	Q_UNUSED(requestID);
	ui->discoverTable->setText( tr("Discover Table") );
	ui->oidBase->setEnabled( true );
}

void MainWindow::on_statusColumn_currentIndexChanged(int index)
{
	bool updateHeaders = false;
	int newStatusCol = ui->statusColumn->itemData(index).toInt();
	int oldStatusCol = mTableColumnInfoList.statusColumnIndex();

	if( oldStatusCol != newStatusCol )
	{
		mTableColumnInfoList.setStatusColumnIndex(newStatusCol);
		if( oldStatusCol != -1 )
		{
			updateStatusColumnValues(oldStatusCol);
			ui->snmpTable->setItemDelegateForColumn(oldStatusCol, Q_NULLPTR);
			updateHeaders = true;
		}
		if( newStatusCol != -1 )
		{
			updateStatusColumnValues(newStatusCol);
			ui->snmpTable->setItemDelegateForColumn(newStatusCol, new QStatusColumnComboBoxDelegate( [this] () {return mSMIVersion;}) );
			updateHeaders = true;
		}
	}
	if( updateHeaders )
		updateHeaderLabel(-1);
}

void MainWindow::on_sendTable_clicked()
{
	int col;
	OID oidBase = ui->oidBase->text().toStdString();
	int statusColumnIndex = mTableColumnInfoList.statusColumnIndex();
	if( oidBase.count() < 2 )
	{
		ui->statusBar->showMessage( tr("OID base for table is too short.") );
		return;
	}
	for( col = 0; col < mTableColumnInfoList.keyColumnCount(); col++ )
	{
		if( mTableColumnInfoList[col].valueType != ASN1TYPE_INTEGER )
		{
			ui->statusBar->showMessage( tr("Key %1 is %2. Currently, only integer key values are suported by the API. Aborting")
										.arg( col+1 )
										.arg( SNMPConstants::asn1TypeName(mTableColumnInfoList[col].valueType)) );
			return;
		}
	}
	while( col < mTableColumnInfoList.count() )
	{
		if( (mTableColumnInfoList[col].valueType != ASN1TYPE_INTEGER) &&
			(mTableColumnInfoList[col].valueType != ASN1TYPE_OCTETSTRING) )
		{
			ui->statusBar->showMessage( tr("Column %1 is %2. Currently, only integer or octet strings values are suported by this Tester program. Aborting")
										.arg( col+1 )
										.arg( SNMPConstants::asn1TypeName(mTableColumnInfoList[col].valueType)) );
			return;
		}
		++col;
	}
	Encoder snmp;
	snmpConn.setAgentHost( ui->agentIP->text(), static_cast<quint16>(ui->agentPort->value()) );
	snmp.setVersion( ui->version->currentData(Qt::UserRole).toInt() );
	snmp.setComunity( ui->comunity->currentText().toStdString() );
	snmp.setRequestType( ASN1TYPE_SetRequestPDU );
	OID keys;
	keys.reserve( static_cast<Int64>(mTableColumnInfoList.keyColumnCount()) );
	for( int row = 0; row < ui->snmpTable->rowCount(); ++row )
	{
		keys.clear();
		for( col = 0; col < mTableColumnInfoList.keyColumnCount(); col++ )
		{
			if( !ui->snmpTable->item(row, col) )
			{
				ui->statusBar->showMessage( tr("Key %1 on row %2 doens't have any value. Row skiped").arg(col).arg(row) );
				break;
			}
			QString txt = ui->snmpTable->item(row, col)->text();
			OIDValue oidValue = static_cast<OIDValue>( ui->snmpTable->item(row, col)->text().toULongLong() );
			if( oidValue == 0 )
			{
				ui->statusBar->showMessage( tr("Key %1 on row %2 doens't have valid value. Row skiped").arg(col).arg(row) );
				break;
			}
			keys.append( oidValue );
		}
		if( keys.count() == 0 )
			break;
		if( statusColumnIndex != -1 )
		{
			ASN1Variable var;
			switch( mTableColumnInfoList[statusColumnIndex].valueType )
			{
			case ASN1TYPE_INTEGER:
				var.setInteger(ui->snmpTable->item(row, statusColumnIndex)->data(Qt::UserRole).toInt());
				break;
			default:
				Q_ASSERT(false);
			}
			snmp.addCellPDUVar( oidBase, keys,
								static_cast<OIDValue>(statusColumnIndex),
								var);
		}
		while( col < mTableColumnInfoList.count() )
		{
			if( !mTableColumnInfoList.isReadOnlycolumn(col) && (col != statusColumnIndex) )
			{
				ASN1Variable var;
				switch( mTableColumnInfoList[col].valueType )
				{
				case ASN1TYPE_INTEGER:
					var.setInteger(ui->snmpTable->item(row, col)->text().toInt());
					break;
				case ASN1TYPE_OCTETSTRING:
					var.setOctetString(ui->snmpTable->item(row, col)->text().toStdString());
					break;
				default:
					Q_ASSERT(false);
				}
				snmp.addCellPDUVar( oidBase, keys, static_cast<OIDValue>(col), var);
				snmp.setRequestID( TABLE_REQUEST_ID + 1 );
			}
			++col;
		}
		if( snmp.varbindList().count() )
			snmpConn.sendRequest( snmp );
		else
			ui->statusBar->showMessage( tr("No columns to modify in row %1. Row skiped").arg(row) );
	}
}
