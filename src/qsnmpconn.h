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

#ifndef SNMPCONN_H
#define SNMPCONN_H

#include <QObject>
#include <QUdpSocket>

#include "lib/snmplib.h"

class SNMPConn : public QObject
{
Q_OBJECT

	QString mAgentAddress;
	quint16 mAgentPort;
	QUdpSocket mAgentSocket;
	bool mIncludeRawData;		// This is usefull for debuging applications.
	OID mTableBaseOID;

	void onDataReceived();

public:
	SNMPConn(quint16 agentPort = 161, const QString &agentAddress = QString(), bool includeRawData = false);

	void setAgentHost(const QString &agentAddress, quint16 agentPort);
	void setIncludeRawData(bool includeRawData = true)	{ mIncludeRawData = includeRawData;	}
	bool includeRawData() const							{ return mIncludeRawData;	}

	void sendRequest(const SNMP::Encoder &snmpDeco);

	void sendGetRequest(int version, const OID &oid, const QString &comunity, int requestID);
	void sendGetRequest(int version, const QString &oid, const QString &comunity, int requestID)
	{
		sendGetRequest(version, OID(oid.toStdString()), comunity, requestID);
	}

	void sendGetNextRequest(int version, const OID &oid, const QString &comunity, int requestID);
	void sendGetNextRequest(int version, const QString &oid, const QString &comunity, int requestID)
	{
		sendGetNextRequest(version, OID(oid.toStdString()), comunity, requestID);
	}

	void sendSetRequest(int version, const OID &oid, const QString &comunity, const ASN1::Variable &asn1Var, int requestID);
	void sendSetRequest(int version, const QString &oid, const QString &comunity, const ASN1::Variable &asn1Var, int requestID)
	{
		sendSetRequest(version, OID(oid.toStdString()), comunity, asn1Var, requestID);
	}

	void discoverTable(int version, const OID &oid, const QString &comunity, int requestID);
	void discoverTable(int version, const QString &oid, const QString &comunity, int requestID)
	{
		discoverTable(version, OID(oid.toStdString()), comunity, requestID);
	}

	void cancelDiscoverTable();
	const OID &tableBaseOID() const	{ return mTableBaseOID;	}
	bool isDiscoveringTable()const	{ return !mTableBaseOID.isEmpty();	}

signals:
	void dataReceived(const SNMP::Encoder &snmp);
	void tableCellReceived(const SNMP::Encoder &snmp);
	void tableReceived(int requestID);
};

#endif // SNMPCONN_H
