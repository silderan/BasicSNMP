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

#include "qsnmpconn.h"

#include <QHostAddress>
#include <QUdpSocket>

using namespace SNMP;

SNMPConn::SNMPConn(QObject *papi, bool includeRawData)
	: QObject(papi)
	, mAgentPort(0)
	, mIncludeRawData(includeRawData)
{
	connect( &mAgentSocket, &QUdpSocket::readyRead, this, &SNMPConn::onDataReceived );
}

SNMPConn::SNMPConn(quint16 agentPort, const QString &agentAddress, bool includeRawData, QObject *papi)
	: QObject(papi)
	, mAgentPort(0)
	, mIncludeRawData(includeRawData)
{
	setAgentHost( agentAddress, agentPort );
	connect( &mAgentSocket, &QUdpSocket::readyRead, this, &SNMPConn::onDataReceived );
}

void SNMPConn::setAgentHost(const QString &agentAddress, quint16 agentPort)
{
	if( agentPort != mAgentPort )
	{
		if( mAgentPort != 0 )
			mAgentSocket.close();
		if( agentPort != 0 )
			Q_ASSERT( mAgentSocket.bind(agentPort, QAbstractSocket::ShareAddress) );
	}
	mAgentAddress = agentAddress;
	mAgentPort = agentPort;
}

void SNMPConn::sendRequest(const Encoder &snmpDeco)
{
	StdByteVector data = snmpDeco.encodeRequest();

	Q_ASSERT( mAgentSocket.writeDatagram( data.chars(), data.count(), QHostAddress(mAgentAddress), mAgentPort) != -1 );
}

void SNMPConn::sendGetRequest(int version, const OID &oid, const QString &comunity, int requestID)
{
	Encoder snmpDeco;
	snmpDeco.setupGetRequest(version, comunity.toStdString(), requestID, oid);
	sendRequest(snmpDeco);
}

void SNMPConn::sendGetNextRequest(int version, const OID &oid, const QString &comunity, int requestID)
{
	Encoder snmpDeco;
	snmpDeco.setupGetNextRequest(version, comunity.toStdString(), requestID, oid);
	sendRequest(snmpDeco);
}

void SNMPConn::sendSetRequest(int version, const OID &oid, const QString &comunity, const ASN1Variable &asn1Var, int requestID)
{
	Encoder snmpDeco;
	snmpDeco.setupSetRequest(version, comunity.toStdString(), requestID, oid, asn1Var);
	sendRequest(snmpDeco);
}

void SNMPConn::discoverTable(int version, const OID &oid, const QString &comunity, int requestID)
{
	Q_ASSERT( !mTableRequestMap.contains(requestID) );
	mTableRequestMap[requestID] = oid;

	sendGetNextRequest(version, oid, comunity, requestID);
}

void SNMPConn::cancelDiscoverTable(int requestID)
{
	mTableRequestMap.remove(requestID);
}

void SNMPConn::onDataReceived()
{
	if( mAgentSocket.hasPendingDatagrams() )
	{
		StdByteVector datagram( static_cast<Int64>(mAgentSocket.pendingDatagramSize()) );
		mAgentSocket.readDatagram( datagram.chars(), datagram.count() );
		Encoder snmp;
		snmp.decodeAll(datagram, includeRawData());

		if( !mTableRequestMap.contains(snmp.requestID()) )
			emit dataReceived(snmp);
		else
		{
			OID tableBaseOID = mTableRequestMap.value(snmp.requestID());
			if( snmp.varbindList().count() &&
				snmp.varbindList().first().oid().startsWith(tableBaseOID) )
			{
				emit tableCellReceived( snmp );
				sendGetNextRequest( snmp.version(),
									snmp.varbindList().first().oid(),
									QString::fromStdString(snmp.comunity()),
									snmp.requestID() );
			}
			else
			{
				cancelDiscoverTable( snmp.requestID() );
				emit tableReceived( snmp.requestID() );
			}
		}
	}
}
