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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "QIniFile.h"

#include "qsnmpconn.h"
#include "qconstantsstrings.h"

namespace Ui
{
    class MainWindow;
}

struct TableColumnInfo
{
	SNMP::ASN1DataType valueType;
	bool readOnly;
	QString name;

	TableColumnInfo(SNMP::ASN1DataType valueType = ASN1TYPE_NULL, bool readOnly = true, const QString &name = "")
		: valueType(valueType)
		, readOnly(readOnly)
		, name(name)
	{	}
	explicit TableColumnInfo(const QString saveString)
	{
		fromSaveString(saveString);
	}
	QString toSaveString()const
	{
		return QString("1,%1,%2,%3,%4,%5")
				.arg(name)
				.arg(static_cast<int>(valueType))
				.arg(readOnly?"readOnly":"writable");
	}
	void fromSaveString(const QString saveString)
	{
		QStringList bits = saveString.split(',');
		if( bits.count() )
			switch( bits[0].toUInt() )
			{
			case 1:
				if( bits.count() != 4 )
					return;
				name = bits[1];
				valueType = static_cast<SNMP::ASN1DataType>(bits[2].toInt());
				readOnly = bits[3] == "readOnly";
				break;
			}
	}
};

class TableColumnInfoList : public QList<TableColumnInfo>
{
	int mStatusColumnIndex;
	int mKeyColumnCount;

public:
	TableColumnInfoList()
		: mStatusColumnIndex(-1)
		, mKeyColumnCount(0)
	{	}
	int statusColumnIndex() const	{ return mStatusColumnIndex;	}
	void setStatusColumnIndex(int i){ mStatusColumnIndex = i;		}

	int keyColumnCount() const		{ return mKeyColumnCount;		}
	int regularColumnCount() const	{ return count() - mKeyColumnCount;	}

	QList<int> keyColumnIndexes()const
	{
		QList<int> keys;

		for( int col = 0; col < keyColumnCount(); ++col )
			keys.append( col );

		return keys;
	}

	QStringList names() const
	{
		QStringList rtn;
		for( const TableColumnInfo &c : *this )
			rtn.append(c.name);
		return rtn;
	}
	bool isKeyColumn(int colIndex) const		{ return colIndex < keyColumnCount();		}
	bool isStatusColumn(int colIndex) const		{ return colIndex == statusColumnIndex();	}
	bool isReadOnlycolumn(int colIndex) const	{ return isKeyColumn(colIndex) || at(colIndex).readOnly;				}

	QStringList descriptions() const
	{
		QStringList rtn;

		for( int col = 0; col < count(); ++col )
		{
			if( col < keyColumnCount() )
				rtn.append( QString("Index %1\nKey %2\n%3\n%4")
							.arg( col )
							.arg( col+1 )
							.arg( SNMPConstants::asn1TypeName(at(col).valueType) )
							.arg( at(col).name) );
			else
				rtn.append( QString("Index %1\nColumn %2\n%3\n%4")
							.arg( col )
							.arg( col+1 - keyColumnCount() )
							.arg( SNMPConstants::asn1TypeName(at(col).valueType) )
							.arg( isStatusColumn(col) ? "Status Column" : at(col).name) );
		}
		return rtn;
	}
	QString toSaveString()const
	{
		QString result = QString("1$%1$%2").arg(mStatusColumnIndex).arg(mKeyColumnCount);
		for( TableColumnInfo c : *this )
			result.append( QString("$%1").arg(c.toSaveString()) );
		return result;
	}
	TableColumnInfoList fromSaveString(const QString &saveString)
	{
		QStringList bits = saveString.split('$', QString::SkipEmptyParts);
		if( bits.count() > 2 )
		{
			switch( bits[0].toInt() )
			{
			case 0:
			{
				int c = 0;
				mStatusColumnIndex = bits[++c].toInt();
				mKeyColumnCount = bits[++c].toInt();
				while( ++c < bits.count() )
					QList::append( TableColumnInfo(bits[c]) );
			}
			break;
			}
		}
		return *this;
	}
	void appendKeyColumn( const TableColumnInfo &tci )
	{
		QList::insert( mKeyColumnCount++, tci );
	}
	void removeKeyColumn(int keyColumnIndex)
	{
		Q_ASSERT( isKeyColumn(keyColumnIndex) );

		removeAt( keyColumnIndex );
		mKeyColumnCount--;
	}
	void appendRegularColumn( const TableColumnInfo &tci )
	{
		QList::append( tci );
	}
	void removeRegularColumn( int columnIndex )
	{
		Q_ASSERT( columnIndex + keyColumnCount() < count() );
		QList::removeAt( columnIndex + keyColumnCount() );
	}
	void clear()
	{
		mKeyColumnCount = 0;
		mStatusColumnIndex = -1;
		QList::clear();
	}
};

class SNMPAPITesterConfigData
{
	QString mFileName;
	QIniData mIniData;

public:
	void load()
	{
		QIniFile::load("SNMPAPITester.ini", &mIniData);
	}
	void save()
	{
		QIniFile::save("SNMPAPITester.ini", mIniData);
	}

	QString agentIP() const						{ return mIniData["snmpAPITester.AgentIP"];		}
	void setAgentIP(const QString &agentIP)		{ mIniData["snmpAPITester.AgentIP"] = agentIP;	}

	quint16 agentPort() const					{ return static_cast<quint16>(mIniData["snmpAPITester.AgentPort"].toUInt());	}
	void setAgentPort(const quint16 &agentPort)	{ mIniData["snmpAPITester.AgentPort"] = QString::number(agentPort);	}

	quint16 trapPort() const					{ return static_cast<quint16>(mIniData["snmpAPITester.TrapPort"].toUInt());	}
	void setTrapPort(const quint16 &trapPort)	{ mIniData["snmpAPITester.TrapPort"] = QString::number(trapPort);	}

	quint8 snmpVersion() const					{ return static_cast<quint8>(mIniData["snmpAPITester.SNMPVersion"].toUInt());	}
	void setSNMPVersion(const quint16 &version)	{ mIniData["snmpAPITester.SNMPVersion"] = QString::number(version);	}

	bool trapEnabled() const					{ return mIniData["snmpAPITester.TrapEnabled"] == "yes";			}
	void setTrapEnabled(bool enabled)			{ mIniData["snmpAPITester.TrapEnabled"] = enabled ? "yes" : "no";	}

	SNMP::ASN1DataType valueType() const			{ return static_cast<SNMP::ASN1DataType>(mIniData["snmpAPITester.valueType"].toInt());		}
	void setValueType(SNMP::ASN1DataType valueType)	{ mIniData["snmpAPITester.valueType"] = QString::number(static_cast<int>(valueType));	}

	QString userName() const					{ return mIniData["snmpAPITester.userName"];		}
	void setUserName(const QString &userName)	{ mIniData["snmpAPITester.userName"] = userName;	}

	QString userPass() const					{ return mIniData["snmpAPITester.userPass"];		}
	void setUserPass(const QString &userPass)	{ mIniData["snmpAPITester.userPass"] = userPass;	}

	QString comunity() const					{ return mIniData["snmpAPITester.Comunity"];		}
	void setComunity(const QString &comunity)	{ mIniData["snmpAPITester.Comunity"] = comunity;	}

	QString oid() const							{ return mIniData["snmpAPITester.OID"];		}
	void setOID(const QString &oid)				{ mIniData["snmpAPITester.OID"] = oid;		}

	QString baseTableOID() const				{ return mIniData["snmpAPITester.baseTableOID"];	}
	void setBaseTableOID(const QString &oid)	{ mIniData["snmpAPITester.baseTableOID"] = oid;		}

	SNMP::SMIVersion smiVersion() const			{ return static_cast<SNMP::SMIVersion>(mIniData["snmpAPITester.tableSMIVersion"].toInt());	}
	void setSMIVersion(SNMP::SMIVersion smi)	{ mIniData["snmpAPITester.tableSMIVersion"] = QString::number(static_cast<int>(smi));			}

	TableColumnInfoList tableColumnsInfoList() const
	{
		return TableColumnInfoList().fromSaveString(mIniData["snmpAPITester.tableColumnsInfoList"] );
	}
	void setTableColumnsInfoList(const TableColumnInfoList &tableColumnsInfoList )
	{
		mIniData["snmpAPITester.tableColumnsInfoList"] = tableColumnsInfoList.toSaveString();
	}
};

class QComboBox;

class MainWindow : public QMainWindow
{
	Q_OBJECT
	Ui::MainWindow *ui;

	SNMPAPITesterConfigData configData;
	SNMPConn snmpConn;
	int mRequestID;
	TableColumnInfoList mTableColumnInfoList;
	SNMP::SMIVersion mSMIVersion;
	QStringList mGetColumnsOidList;

	SNMP::ASN1DataType currentValueType(QComboBox *cb)const;
	SNMP::ASN1DataType valueType(QComboBox *cb, int index)const;
	void selectValueType(QComboBox *cb, SNMP::ASN1DataType valueType);
	void setupValueTypeComboBox(QComboBox *cb, SNMP::ASN1DataType selected);
	static const QList<SNMP::ASN1DataType> &asn1Types();
	void updateHeaderLabel(int col);
	void updateLocalIndexComboBox();
	void addReplyRow(const SNMP::OID &oid, const QString &valueType, const QString &value, const QString &rawValue);
	void addReplyRow(const SNMP::PDUVarbind &varbind );

//	void addSNMPTableColumn(int widgetColumn, ASN1Type colType, bool readOnly, const QString colName);
	void setupSNMPTableTab();

	int rowOf(SNMP::OID keys);
	void updateStatusColumnValues(int col);
	void updateSnmpTableCell(const SNMP::TableInfo &tableInfo);

	void getColumns_RequestFirst();
public:
	explicit MainWindow(QWidget *parent = nullptr);
	~MainWindow();

private slots:
	void onSNMPDataReceived(const SNMP::Encoder &snmp);
	void onTableCellReceived(const SNMP::Encoder &snmp);
	void onTableReceived(int requestID);

	void onKeyColumnCountChanged(int count);
	void onRegularColumnCountChanged(int regularColumnCount);
	void onReadOnlyChanged(int newState);
	void onColumnValueTypeChanged(int valueTypeIndex);
	void onRowsChanged(int rows);
	void onSMIVersionChanged(int index);

	void on_sendGetRequest_clicked();
	void on_sendGetNextRequest_clicked();
	void on_sendSetRequest_clicked();
	void on_replyTable_cellDoubleClicked(int row, int column);
	void on_version_currentIndexChanged(int index);
	void on_trapEnabled_stateChanged(int checked);
	void on_columnLocalIndex_currentIndexChanged(int index);
	void on_columnAlias_textChanged(const QString &text);
	void on_discoverTable_clicked();
	void on_statusColumn_currentIndexChanged(int index);
	void on_sendTable_clicked();
	void on_getColumnsStart_clicked();
};

#endif // MAINWINDOW_H
