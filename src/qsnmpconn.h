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
	bool includeRasData() const							{ return mIncludeRawData;	}

	void sendRequest(const SNMP::Encoder &snmpDeco);
	void sendGetRequest(int version, const OID &oid, const QString &comunity, int requestID);
	void sendGetNextRequest(int version, const OID &oid, const QString &comunity, int requestID);
	void sendSetRequest(int version, const OID &oid, const QString &comunity, const ASN1::Variable &asn1Var, int requestID);

	void discoverTable(int version, const OID &oid, const QString &comunity, int requestID);
	void cancelDiscoverTable();
	const OID &tableBaseOID() const	{ return mTableBaseOID;	}
	bool isDiscoveringTable()const	{ return !mTableBaseOID.isEmpty();	}

signals:
	void dataReceived(const SNMP::Encoder &snmp);
	void tableCellReceived(const SNMP::Encoder &snmp);
	void tableReceived(int requestID);
};

#endif // SNMPCONN_H
