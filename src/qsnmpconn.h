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
	quint16 mTrapPort;
	QUdpSocket mTrapSocket;
	bool mIncludeRawData;		// This is usefull for debuging applications.

	struct RequestInfo
	{
		int requestID;
		SNMP::OID requestOID;
		SNMP::OID initialOID;	// Only for table requests.
		SNMP::Version version;
		QString comunity;
		SNMP::ASN1Variable asn1Var;	// Only for Set requests
		enum RequestType
		{
			get,
			next,
			set,
			table
		} requestType;
		enum RequestStatus
		{
			idle,
			requested
		}requestStatus;

		RequestInfo(const RequestInfo &other) = default;
		RequestInfo()
		 : requestID(0)
		 , requestOID(0)
		 , initialOID(0)
		 , version(SNMP::V1)
		 , asn1Var()
		 , requestType(RequestType::get)
		 , requestStatus(RequestStatus::idle)
		{	}
		bool isIdle()const		{ return requestStatus == RequestStatus::idle;		}
		bool isRequested()const { return requestStatus == RequestStatus::requested;	}
	};

	class RequestInfoList : public QList<RequestInfo>
	{
	public:
		RequestInfo currentRequestInfo() const
		{
			return count() ? first() : RequestInfo();
		}
		bool contains(const int requestID) const
		{
			for( const RequestInfo &t : *this )
				if( t.requestID == requestID )
					return true;
			return false;
		}
		SNMP::OID initialOID(const int &requestID) const
		{
			for( const RequestInfo &t : *this )
				if( t.requestID == requestID )
					return t.initialOID;
			return SNMP::OID();
		}
		bool remove( const int &requestID )
		{
			int i = 0;
			for( const RequestInfo &t : *this )
			{
				if( t.requestID == requestID )
				{
					removeAt(i);
					return true;
				}
				i++;
			}
			return false;
		}
	}mRequestList;

	void play();
	void onDataReceived();
	void onTrapReceived();

public:
	explicit SNMPConn(QObject *papi, bool includeRawData = false);
	explicit SNMPConn(quint16 agentPort = 161, const QString &agentAddress = QString(), bool includeRawData = false, QObject *papi = Q_NULLPTR);

	const QString &agentAddress() const	{ return mAgentAddress;	}
	quint16 agentPort() const			{ return mAgentPort;	}
	void setAgentHost(const QString &agentAddress, quint16 agentPort);
	void setTrapHost(quint16 trapPort);
	void setIncludeRawData(bool includeRawData = true)	{ mIncludeRawData = includeRawData;	}
	bool includeRawData() const							{ return mIncludeRawData;	}

	void sendRequest(const SNMP::Encoder &snmpDeco);

	void sendGetRequest(int version, const SNMP::OID &oid, const QString &comunity, int requestID);
	void sendGetRequest(int version, const QString &oid, const QString &comunity, int requestID)
	{
		sendGetRequest(version, SNMP::OID(oid.toStdString()), comunity, requestID);
	}
	void appendSendGetRequest(int version, const SNMP::OID &oid, const QString &comunity, int requestID);

	void sendGetNextRequest(int version, const SNMP::OID &oid, const QString &comunity, int requestID);
	void sendGetNextRequest(int version, const QString &oid, const QString &comunity, int requestID)
	{
		sendGetNextRequest(version, SNMP::OID(oid.toStdString()), comunity, requestID);
	}
	void appendSendGetNextRequest(int version, const SNMP::OID &oid, const QString &comunity, int requestID);

	void sendSetRequest(int version, const SNMP::OID &oid, const QString &comunity, const SNMP::ASN1Variable &asn1Var, int requestID);
	void sendSetRequest(int version, const QString &oid, const QString &comunity, const SNMP::ASN1Variable &asn1Var, int requestID)
	{
		sendSetRequest(version, SNMP::OID(oid.toStdString()), comunity, asn1Var, requestID);
	}
	void appendSendSetRequest(int version, const SNMP::OID &oid, const QString &comunity, const SNMP::ASN1Variable &asn1Var, int requestID);

	void discoverTable(int version, const SNMP::OID &oid, const QString &comunity, int requestID);
	void discoverTable(int version, const QString &oid, const QString &comunity, int requestID)
	{
		discoverTable(version, SNMP::OID(oid.toStdString()), comunity, requestID);
	}

	void cancelDiscoverTable(int requestID);
	SNMP::OID tableBaseOID(int requestID) const			{ return mRequestList.initialOID(requestID);	}
	bool isDiscoveringTable(int requestID) const		{ return mRequestList.contains(requestID);		}

signals:
	void dataReceived(const SNMP::Encoder &snmp);
	void trapReceived(const SNMP::Encoder &snmp);
	void tableCellReceived(const SNMP::Encoder &snmp);
	void tableReceived(int requestID);
};

#endif // SNMPCONN_H
