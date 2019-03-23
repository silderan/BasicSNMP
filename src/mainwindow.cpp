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

const QList<ASN1::DataType> &MainWindow::asn1Types()
{
	static QList<ASN1::DataType> setRequestTypes = QList<ASN1::DataType>() << ASN1TYPE_INTEGER
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

class QStatusColumnComboBoxDelegate : public QStyledItemDelegate
{
	std::function<SNMP::SMIVersion()> mGetSMIVer;

public:
	QStatusColumnComboBoxDelegate(std::function<SNMP::SMIVersion()> getSMIVer)
		: mGetSMIVer(getSMIVer)
	{

	}
	QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
	{
		Q_UNUSED(option);
		Q_UNUSED(index);

		switch( mGetSMIVer() )
		{
		case SNMP::SMIVersion::SMIv1:
		{
			QComboBox *cb = new QComboBox(parent);
			for( auto it : SNMP::entryStatusInfoMap() )
				cb->addItem( it.second.data(), static_cast<int>(it.first) );

			return cb;
		}
		case SNMP::SMIVersion::SMIv2:
		{
			QComboBox *cb = new QComboBox(parent);
			for( auto it : SNMP::entryStatusInfoMap() )
				cb->addItem( it.second.data(), static_cast<int>(it.first) );

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

ASN1::DataType MainWindow::currentValueType(QComboBox *cb) const
{
	return valueType(cb, cb->currentIndex());
}

ASN1::DataType MainWindow::valueType(QComboBox *cb, int index) const
{
	int value = cb->itemData(index, Qt::UserRole).value<int>();
	return static_cast<ASN1::DataType>(value);
}

void MainWindow::selectValueType(QComboBox *cb, ASN1::DataType valueType)
{
	for( int i = 0; i < cb->count(); ++i )
		if( this->valueType(cb, i) == valueType )
		{
			cb->setCurrentIndex(i);
			break;
		}
}

void MainWindow::setupValueTypeComboBox(QComboBox *cb, ASN1::DataType selected)
{
	for( ASN1::DataType dataType : asn1Types() )
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

void MainWindow::addReplyRow(const SNMP::PDUVarbind &varbind)
{
	addReplyRow( varbind.oid(),
				 SNMPConstants::asn1TypeName(varbind.type()),
				 SNMPConstants::asn1PrintableValue(varbind),
				 SNMPConstants::printableRawData(varbind) );
}

void MainWindow::onSNMPDataReceived(const SNMP::Encoder &snmp)
{
	ui->replyTable->setRowCount(0);

	if( !snmp.varbindList().count() )
		ui->statusBar->showMessage( tr("Ningun dato recibido. Error: %1").arg(SNMPConstants::printableErrorCode(snmp)) );
	else
	{
		for( const SNMP::PDUVarbind &varbind : snmp.varbindList() )
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
	ASN1::Variable asn1Var;
	switch( ui->valueType->currentData().toInt() )
	{
	case ASN1TYPE_BOOLEAN:
		asn1Var.setBoolean( ui->value->text()=="true" );
		break;
	case ASN1TYPE_INTEGER:
		asn1Var.setInteger( ui->value->text().toInt() );
		break;
	case ASN1TYPE_OCTETSTRING:
		asn1Var.setOctetString( ui->value->text().toStdString() );
		break;
	default:
		ui->statusBar->showMessage( tr("Envio de datos tipo %1 no implementado aun")
									.arg( SNMPConstants::asn1TypeName( static_cast<ASN1::DataType>(ui->valueType->currentData().toInt()))) );
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

	ui->smiVersion->addItem( "SMIv1 (EntryStatus)", static_cast<int>(SNMP::SMIVersion::SMIv1) );
	ui->smiVersion->addItem( "SMIv2 (RowStatus)", static_cast<int>(SNMP::SMIVersion::SMIv2) );
	ui->smiVersion->setCurrentIndex( mSMIVersion - SNMP::SMIVersion::SMIv1 );

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
	switch( mSMIVersion = static_cast<SNMP::SMIVersion>( ui->smiVersion->itemData(index).toInt()) )
	{
	case SNMP::SMIVersion::SMIUnd:
		ui->statusColumn->setEnabled(false);
		ui->statusColumnLabel->setText( tr("Choose SMIVersion") );
		break;
	case SNMP::SMIVersion::SMIv1:
		ui->statusColumn->setEnabled(true);
		ui->statusColumnLabel->setText( tr("Column for EntryStatus") );
		break;
	case SNMP::SMIVersion::SMIv2:
		ui->statusColumn->setEnabled(true);
		ui->statusColumnLabel->setText( tr("Column for StatusRow") );
		break;
	}
	updateHeaderLabel(-1);
	updateStatusColumnValues(-1);
}

void MainWindow::on_getTable_clicked()
{
	if( !ui->oidBase->text().length() )
		ui->statusBar->showMessage( tr("Falta el OID base de la tabla") );
	else
	{
//		SNMPDecoder snmp;
//		snmpConn.setAgentHost( ui->agentIP->text(), static_cast<quint16>(ui->agentPort->value()) );
//		snmp.setVersion( ui->version->currentData(Qt::UserRole).toInt() );
//		snmp.setComunity( ui->comunity->currentText() );
//		snmp.setRequestID( ++mRequestID );
//		snmp.setRequestType( ASN1TYPE_GetNextRequestPDU );

//		for( int col = 0, i = 0; i < ui->snmpTable->columnCount(); ++i )
//			if( !mTableColumnInfoList[i].isKeyColumn() )
//			{
//				QString oid = QString("%1.%2").arg(ui->oidBase->text()).arg(++col);
//				snmp.addPDUVar( PDUVarbind(oid) );
//			}

//		snmpConn.sendRequest( snmp );
	}
}

void MainWindow::on_discoverTable_clicked()
{
	if( snmpConn.isDiscoveringTable() )
	{
		snmpConn.cancelDiscoverTable();
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
		snmpConn.discoverTable( ui->version->currentData(Qt::UserRole).toInt(), ui->oidBase->text(), ui->comunity->currentText(), ++mRequestID );
	}
}

int MainWindow::matchKey(OID keys)
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
	// Not found.
	if( row == ui->snmpTable->rowCount() )
	{
		ui->rows->setValue(row+1);
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
			case SNMP::SMIVersion::SMIv1:
				ui->snmpTable->item(row, col)->setText( SNMP::qEntryStatusName( static_cast<SNMP::EntryStatus>(statusRow)) );
				break;
			case SNMP::SMIVersion::SMIv2:
				ui->snmpTable->item(row, col)->setText( SNMP::qStatusRowName( static_cast<SNMP::StatusRow>(statusRow)) );
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

void MainWindow::onTableCellReceived(const SNMP::Encoder &snmp)
{
	ui->statusBar->showMessage( SNMPConstants::printableErrorCode(snmp) );
	SNMP::TableInfo tableInfo(snmpConn.tableBaseOID(), snmp);

	if( tableInfo.keyIndexes.count() != ui->keyCount->value() )
	{
		ui->keyCount->setValue( static_cast<int>(tableInfo.keyIndexes.count()) );
	}

	if( tableInfo.column >= static_cast<UInt64>(ui->columnCount->value()) )
	{
		ui->columnCount->setValue( static_cast<int>(tableInfo.column) );
		mTableColumnInfoList[tableInfo.column + tableInfo.keyIndexes.count() - 1].valueType = tableInfo.varbind.type();
		updateHeaderLabel(-1);
	}

	int row = matchKey( tableInfo.keyIndexes );
	ui->snmpTable->setItem( row, tableInfo.column + tableInfo.keyIndexes.count() - 1,
							new QTableWidgetItem( SNMPConstants::asn1PrintableValue(tableInfo.varbind) ) );
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
	SNMP::Encoder snmp;
	snmpConn.setAgentHost( ui->agentIP->text(), static_cast<quint16>(ui->agentPort->value()) );
	snmp.setVersion( ui->version->currentData(Qt::UserRole).toInt() );
	snmp.setComunity( ui->comunity->currentText().toStdString() );
	snmp.setRequestType( ASN1TYPE_SetRequestPDU );
	OID keys;
	keys.reserve( static_cast<UInt64>(mTableColumnInfoList.keyColumnCount()) );
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
			ASN1::Variable var;
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
				ASN1::Variable var;
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
				snmp.setRequestID( ++mRequestID );
			}
			++col;
		}
		if( snmp.varbindList().count() )
			snmpConn.sendRequest( snmp );
		else
			ui->statusBar->showMessage( tr("No columns to modify in row %1. Row skiped").arg(row) );
	}
}
