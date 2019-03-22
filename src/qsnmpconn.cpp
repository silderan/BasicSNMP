#include "qsnmpconn.h"

#include <QHostAddress>
#include <QUdpSocket>

SNMPConn::SNMPConn(quint16 agentPort, const QString &agentAddress, bool includeRawData)
	: mAgentPort(0)
	, mIncludeRawData(includeRawData)
{
	setAgentHost(agentAddress, agentPort);
	connect( &mAgentSocket, &QUdpSocket::readyRead, this, &SNMPConn::onDataReceived );
}

void SNMPConn::setAgentHost(const QString &agentAddress, quint16 agentPort)
{
	if( agentPort != mAgentPort )
	{
		if( mAgentPort != 0 )
			mAgentSocket.close();
		if( agentPort != 0 )
			Q_ASSERT( mAgentSocket.bind(agentPort) );
	}
	mAgentAddress = agentAddress;
	mAgentPort = agentPort;
}

void SNMPConn::sendRequest(const SNMP::Encoder &snmpDeco)
{
	StdCharVector data = snmpDeco.encodeRequest();

	Q_ASSERT( mAgentSocket.writeDatagram(data.data(), data.count(), QHostAddress(mAgentAddress), mAgentPort) != -1 );
}

void SNMPConn::sendGetRequest(int version, const OID &oid, const QString &comunity, int requestID)
{
	SNMP::Encoder snmpDeco;
	snmpDeco.setupGetRequest(version, comunity.toStdString(), requestID, oid);
	sendRequest(snmpDeco);
}

void SNMPConn::sendGetNextRequest(int version, const OID &oid, const QString &comunity, int requestID)
{
	SNMP::Encoder snmpDeco;
	snmpDeco.setupGetNextRequest(version, comunity.toStdString(), requestID, oid);
	sendRequest(snmpDeco);
}

void SNMPConn::sendSetRequest(int version, const OID &oid, const QString &comunity, const ASN1::Variable &asn1Var, int requestID)
{
	SNMP::Encoder snmpDeco;
	snmpDeco.setupSetRequest(version, comunity.toStdString(), requestID, oid, asn1Var);
	sendRequest(snmpDeco);
}

void SNMPConn::discoverTable(int version, const OID &oid, const QString &comunity, int requestID)
{
	Q_ASSERT(mTableBaseOID.isEmpty());
	mTableBaseOID = oid;
	sendGetNextRequest(version, oid, comunity, requestID);
}

void SNMPConn::cancelDiscoverTable()
{
	mTableBaseOID.clear();
}

void SNMPConn::onDataReceived()
{
	if( mAgentSocket.hasPendingDatagrams() )
	{
		StdCharVector datagram( static_cast<long long>(mAgentSocket.pendingDatagramSize()) );
		mAgentSocket.readDatagram( datagram.data(), datagram.count() );
		SNMP::Encoder snmpDeco;
		snmpDeco.decodeAll(datagram, includeRasData());
		if( mTableBaseOID.isEmpty() )
			emit dataReceived(snmpDeco);
		else
		{
			if( snmpDeco.varbindList().count() &&
				snmpDeco.varbindList().first().oid().startsWith(mTableBaseOID) )
			{
				emit tableCellReceived(snmpDeco);
				sendGetNextRequest(snmpDeco.version(), snmpDeco.varbindList().first().oid(), QString::fromStdString(snmpDeco.comunity()), snmpDeco.requestID()+1);
			}
			else
			{
				cancelDiscoverTable();
				emit tableReceived(snmpDeco.requestID());
			}
		}
	}
}
