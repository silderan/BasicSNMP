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

#ifndef SNMPENCODER_H
#define SNMPENCODER_H

#ifdef QT_CORE_LIB
#include <QString>
#endif

#include "asn1variable.h"
#include "asn1types.h"
#include "asn1encoder.h"

#include "pduvarbind.h"

namespace SNMP {

class Encoder
{
	int mVersion;
	std::string mComunity;
	int mRequestID;
	ASN1::Encoder::ErrorCode mErrorCode;
	int mErrorObjectIndex;
	ASN1::DataType mRequestType;
	PDUVarbindList mVarbindList;

public:
	Encoder();

	int version() const				{ return mVersion;		}
	void setVersion(int v)			{ mVersion = v;			}

	const std::string &comunity() const		{ return mComunity;	}
	void setComunity(const std::string &c)	{ mComunity = c;	}
	void setComunity(const StdCharVector &c){ mComunity = std::string(c.data(), c.size()); }

	void setComunity(const char *c)			{ mComunity = std::string(c);	}

#ifdef QT_CORE_LIB
	QString qComunity() const				{ return QString::fromStdString(mComunity);	}
	void setComunity(const QString &c)		{ mComunity = c.toStdString();	}
#endif
	int requestID() const		{ return mRequestID;	}
	void setRequestID(int r)	{ mRequestID = r;		}

	ASN1::Encoder::ErrorCode errorCode() const		{ return mErrorCode;	}
	void setErrorCode(ASN1::Encoder::ErrorCode c)	{ mErrorCode = c;		}

	int errorObjectIndex() const		{ return mErrorObjectIndex;	}
	void setErrorObjectIndex(int i)		{ mErrorObjectIndex = i;	}

	ASN1::DataType requestType() const				{ return mRequestType;			}
	void setRequestType(ASN1::DataType requestType)	{ mRequestType = requestType;	}

	void addPDUVar(const PDUVarbind &pduVarbind);
	void addCellPDUVar(OID oidBase, const OID &keyValues, OIDValue oidColumn, const ASN1::Variable &asn1Var);

	const PDUVarbindList &varbindList()	const	{ return mVarbindList;	}
	void setObjectIdentifier(const OID &oid)	{ addPDUVar(oid);	}

	void setupRequest(int version, const std::string &comunity, int requestID, ASN1::DataType requestCode, const PDUVarbindList &varbindList);
	void setupRequest(int version, const std::string &comunity, int requestID, ASN1::DataType requestCode, const OIDList &oidList);
	void setupGetRequest(int version, const std::string &comunity, int requestID, const OID &oid);
	void setupGetRequest(int version, const std::string &comunity, int requestID, const OIDList &oidList);
	void setupGetNextRequest(int version, const std::string &comunity, int requestID, const OID &oid);
	void setupGetNextRequest(int version, const std::string &comunity, int requestID, const OIDList &oidList);

	void setupSetRequest(int version, const std::string &comunity, int requestID, const PDUVarbindList &varbindList);
	void setupSetRequest(int version, const std::string &comunity, int requestID, const OID &oid, const ASN1::Variable &asn1Var);

	void setError(ASN1::Encoder::ErrorCode code, int index)	{ mErrorCode = code; mErrorObjectIndex = index;}

	bool decodeAll(const StdCharVector &ba, bool includeRawData);
	StdCharVector encodeRequest() const;
};

} // namespace SNMP
#endif // SNMPENCODER_H
