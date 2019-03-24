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

#include "asn1variable.h"
#include "asn1types.h"
#include "asn1encoder.h"

#include "pduvarbind.h"

namespace SNMP {

class Encoder
{
	Int32 mVersion;
	StdString mComunity;
	int mRequestID;
	ASN1Encoder::ErrorCode mErrorCode;
	int mErrorObjectIndex;
	ASN1DataType mRequestType;
	PDUVarbindList mVarbindList;

public:
	Encoder();

	int version() const				{ return mVersion;		}
	void setVersion(int v)			{ mVersion = v;			}

	const StdString &comunity() const		{ return mComunity;	}
	void setComunity(const StdString &c)	{ mComunity = c;	}
	void setComunity(const StdByteVector &c){ mComunity = c.toStdString(); }

	void setComunity(const char *c)			{ mComunity = StdString(c);	}

	int requestID() const		{ return mRequestID;	}
	void setRequestID(int r)	{ mRequestID = r;		}

	ASN1Encoder::ErrorCode errorCode() const		{ return mErrorCode;	}
	void setErrorCode(ASN1Encoder::ErrorCode c)	{ mErrorCode = c;		}

	int errorObjectIndex() const		{ return mErrorObjectIndex;	}
	void setErrorObjectIndex(int i)		{ mErrorObjectIndex = i;	}

	ASN1DataType requestType() const				{ return mRequestType;			}
	void setRequestType(ASN1DataType requestType)	{ mRequestType = requestType;	}

	void addPDUVar(const PDUVarbind &pduVarbind);
	void addCellPDUVar(OID oidBase, const OID &keyValues, OIDValue oidColumn, const ASN1Variable &asn1Var);

	const PDUVarbindList &varbindList()	const	{ return mVarbindList;	}
	void setObjectIdentifier(const OID &oid)	{ addPDUVar(oid);	}

	void setupRequest(int version, const StdString &comunity, int requestID, ASN1DataType requestCode, const PDUVarbindList &varbindList);
	void setupRequest(int version, const StdString &comunity, int requestID, ASN1DataType requestCode, const OIDList &oidList);
	void setupGetRequest(int version, const StdString &comunity, int requestID, const OID &oid);
	void setupGetRequest(int version, const StdString &comunity, int requestID, const OIDList &oidList);
	void setupGetNextRequest(int version, const StdString &comunity, int requestID, const OID &oid);
	void setupGetNextRequest(int version, const StdString &comunity, int requestID, const OIDList &oidList);

	void setupSetRequest(int version, const StdString &comunity, int requestID, const PDUVarbindList &varbindList);
	void setupSetRequest(int version, const StdString &comunity, int requestID, const OID &oid, const ASN1Variable &asn1Var);

	void setError(ASN1Encoder::ErrorCode code, int index)	{ mErrorCode = code; mErrorObjectIndex = index;}

	bool decodeAll(const StdByteVector &ba, bool includeRawData);
	StdByteVector encodeRequest() const;
};

} // namespace SNMP
#endif // SNMPENCODER_H
