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

	QMap<int, SNMP::OID> mTableRequestMap;

	void onDataReceived();

public:
	explicit SNMPConn(QObject *papi, bool includeRawData = false);
	explicit SNMPConn(quint16 agentPort = 161, const QString &agentAddress = QString(), bool includeRawData = false, QObject *papi = Q_NULLPTR);

	const QString &agentAddress() const	{ return mAgentAddress;	}
	quint16 agentPort() const			{ return mAgentPort;	}
	void setAgentHost(const QString &agentAddress, quint16 agentPort);
	void setIncludeRawData(bool includeRawData = true)	{ mIncludeRawData = includeRawData;	}
	bool includeRawData() const							{ return mIncludeRawData;	}

	void sendRequest(const SNMP::Encoder &snmpDeco);

	void sendGetRequest(int version, const SNMP::OID &oid, const QString &comunity, int requestID);
	void sendGetRequest(int version, const QString &oid, const QString &comunity, int requestID)
	{
		sendGetRequest(version, SNMP::OID(oid.toStdString()), comunity, requestID);
	}

	void sendGetNextRequest(int version, const SNMP::OID &oid, const QString &comunity, int requestID);
	void sendGetNextRequest(int version, const QString &oid, const QString &comunity, int requestID)
	{
		sendGetNextRequest(version, SNMP::OID(oid.toStdString()), comunity, requestID);
	}

	void sendSetRequest(int version, const SNMP::OID &oid, const QString &comunity, const SNMP::ASN1Variable &asn1Var, int requestID);
	void sendSetRequest(int version, const QString &oid, const QString &comunity, const SNMP::ASN1Variable &asn1Var, int requestID)
	{
		sendSetRequest(version, SNMP::OID(oid.toStdString()), comunity, asn1Var, requestID);
	}

	void discoverTable(int version, const SNMP::OID &oid, const QString &comunity, int requestID);
	void discoverTable(int version, const QString &oid, const QString &comunity, int requestID)
	{
		discoverTable(version, SNMP::OID(oid.toStdString()), comunity, requestID);
	}

	void cancelDiscoverTable(int requestID);
	SNMP::OID tableBaseOID(int requestID) const			{ return mTableRequestMap.value(requestID);		}
	bool isDiscoveringTable(int requestID) const	{ return mTableRequestMap.contains(requestID);	}

signals:
	void dataReceived(const SNMP::Encoder &snmp);
	void tableCellReceived(const SNMP::Encoder &snmp);
	void tableReceived(int requestID);
};

#endif // SNMPCONN_H
