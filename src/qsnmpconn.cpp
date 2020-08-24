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
	, mTrapPort(0)
	, mIncludeRawData(includeRawData)
{
	setAgentHost( agentAddress, agentPort );
	connect( &mAgentSocket, &QUdpSocket::readyRead, this, &SNMPConn::onDataReceived );
	connect( &mTrapSocket, &QUdpSocket::readyRead, this, &SNMPConn::onTrapReceived );
}

void SNMPConn::setAgentHost(const QString &agentAddress, quint16 agentPort)
{
	if( (agentPort != mAgentPort) || (agentAddress != mAgentAddress) )
	{
		if( mAgentPort != 0 )
			mAgentSocket.close();
		if( agentPort != 0 )
			mAgentSocket.bind(agentPort, QAbstractSocket::ShareAddress | QAbstractSocket::ReuseAddressHint);
		mAgentAddress = agentAddress;
		mAgentPort = agentPort;
	}
}

void SNMPConn::setTrapHost(quint16 trapPort)
{
	if( mTrapPort != trapPort )
	{
		if( mTrapPort != 0 )
			mTrapSocket.close();
		if( trapPort != 0 )
			mTrapSocket.bind(trapPort, QAbstractSocket::ShareAddress | QAbstractSocket::ReuseAddressHint);
		mTrapPort = trapPort;
	}
}

void SNMPConn::sendRequest(const Encoder &snmpDeco)
{
	StdByteVector data = snmpDeco.encodeRequest();
	qint64 writtenBytes = mAgentSocket.writeDatagram(data.chars(), data.count(), QHostAddress(mAgentAddress), mAgentPort);

	Q_ASSERT( writtenBytes != -1 );
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
	Q_ASSERT( !mTableRequestList.contains(requestID) );
	mTableRequestList.append(RequestInfo());
	RequestInfo &ri = mTableRequestList.last();
	ri.initialOID = oid;
	ri.requestOID = oid;
	ri.requestType = RequestInfo::RequestType::table;
	ri.requestID = requestID;
	ri.version = SNMP::Version(version);
	ri.comunity = comunity;

//	qDebug() << "Discovering table" << requestID;
	play();
}

void SNMPConn::cancelDiscoverTable(int requestID)
{
	qDebug() << "Canceled table with requestID=" << requestID;
	mTableRequestList.remove(requestID);
	if( mTableRequestList.count() )
		play();
}

void SNMPConn::play()
{
	if( mTableRequestList.count() && mTableRequestList.first().isIdle() )
	{
		const RequestInfo &ri = mTableRequestList.first();
		switch( ri.requestType )
		{
		case RequestInfo::RequestType::exact:
			sendGetRequest(ri.version, ri.requestOID, ri.comunity, ri.requestID);
			break;
		case RequestInfo::RequestType::set:
			sendSetRequest(ri.version, ri.requestOID, ri.comunity, ri.asn1Var, ri.requestID);
			break;
		case RequestInfo::RequestType::next:
		case RequestInfo::RequestType::table:
			sendGetNextRequest(ri.version, ri.requestOID, ri.comunity, ri.requestID);
			break;
		}
	}
}

void SNMPConn::onDataReceived()
{
	if( mAgentSocket.hasPendingDatagrams() )
	{
		StdByteVector datagram( static_cast<Int64>(mAgentSocket.pendingDatagramSize()) );
		mAgentSocket.readDatagram( datagram.chars(), datagram.count() );
		Encoder snmp;
		snmp.decodeAll(datagram, includeRawData());

		if( !mTableRequestList.contains(snmp.requestID()) )
			emit dataReceived(snmp);
		else
		{
			RequestInfo &ri = mTableRequestList.first();
			Q_ASSERT(ri.requestID == snmp.requestID());

			if( snmp.varbindList().count() &&
				snmp.varbindList().first().oid().startsWith(ri.initialOID) )
			{
				emit tableCellReceived( snmp );
				ri.requestOID = snmp.varbindList().first().oid();
				play();
			}
			else
			{
				cancelDiscoverTable( snmp.requestID() );
				emit tableReceived( snmp.requestID() );
			}
		}
	}
}

void SNMPConn::onTrapReceived()
{
	if( mTrapSocket.hasPendingDatagrams() )
	{
		StdByteVector datagram( static_cast<Int64>(mAgentSocket.pendingDatagramSize()) );
		mAgentSocket.readDatagram( datagram.chars(), datagram.count() );
		Encoder snmp;
		snmp.decodeAll(datagram, includeRawData());
		emit trapReceived(snmp);
	}
}
