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

#include "snmpencoder.h"

namespace SNMP {

Encoder::Encoder()
	: mVersion(0)
	, mRequestID(0)
	, mErrorCode(ASN1::Encoder::ErrorCode::NoError)
	, mErrorObjectIndex(0)
	, mRequestType(0)
{
}

void Encoder::addPDUVar(const PDUVarbind &pduVarbind)
{
	mVarbindList.append(pduVarbind);
}

void Encoder::addCellPDUVar(OID oidBase, const OID &keyValues, OIDValue oidColumn, const ASN1::Variable &asn1Var )
{
	oidBase.push_back(oidColumn);
	for( auto k : keyValues )
		oidBase.push_back(k);
	addPDUVar( PDUVarbind(oidBase, asn1Var) );
}

void Encoder::setupRequest(int version, const StdString &comunity, int requestID, ASN1::DataType requestCode, const PDUVarbindList &varbindList)
{
	assert( (requestCode == ASN1TYPE_GetRequestPDU) || (requestCode == ASN1TYPE_GetNextRequestPDU) || (requestCode == ASN1TYPE_SetRequestPDU));

	mVersion = version;
	mComunity = comunity;
	mRequestID = requestID;
	mErrorCode = ASN1::Encoder::ErrorCode::NoError;
	mErrorObjectIndex = 0;
	mRequestType = requestCode;
	mVarbindList = varbindList;
}

void Encoder::setupRequest(int version, const StdString &comunity, int requestID, ASN1::DataType requestCode, const OIDList &oidList)
{
	mVarbindList.clear();

	for( const OID &oid : oidList )
		mVarbindList.append( PDUVarbind(oid, ASN1::Variable()) );

	return setupRequest( version, comunity, requestID, requestCode, mVarbindList );
}

void Encoder::setupGetRequest(int version, const StdString &comunity, int requestID, const OID &oid)
{
	return setupRequest(version, comunity, requestID, ASN1TYPE_GetRequestPDU, OIDList(oid) );
}

void Encoder::setupGetRequest(int version, const StdString &comunity, int requestID, const OIDList &oidList)
{
	return setupRequest(version, comunity, requestID, ASN1TYPE_GetRequestPDU, oidList);
}

void Encoder::setupGetNextRequest(int version, const StdString &comunity, int requestID, const OID &oid)
{
	return setupRequest(version, comunity, requestID, ASN1TYPE_GetNextRequestPDU, OIDList(oid));
}

void Encoder::setupGetNextRequest(int version, const StdString &comunity, int requestID, const OIDList &oidList)
{
	return setupRequest(version, comunity, requestID, ASN1TYPE_GetNextRequestPDU, oidList);
}

void Encoder::setupSetRequest(int version, const StdString &comunity, int requestID, const PDUVarbindList &varbindList)
{
	return setupRequest(version, comunity, requestID, ASN1TYPE_SetRequestPDU, varbindList);
}

void Encoder::setupSetRequest(int version, const StdString &comunity, int requestID, const OID &oid, const ASN1::Variable &asn1Var)
{
	return setupSetRequest(version, comunity, requestID, PDUVarbindList() << PDUVarbind(oid, asn1Var) );
}

bool Encoder::decodeAll(const StdByteVector &ba, bool includeRawData)
{
	ASN1::Variable asn1Var;
	Int64 pos = 0;
	Int64 length;

	mErrorCode = ASN1::Encoder::ErrorCode::NoError;
	mErrorObjectIndex = 0;
	if( !ASN1::Encoder::decodeSequence(mErrorCode, ba, pos, length) )
		return false;
	if( !ASN1::Encoder::decodeInteger(mErrorCode, mVersion, ba, pos, true) )
		return false;

	// Comunity.
	StdByteVector comunity;
	if( !ASN1::Encoder::decodeOctetString(mErrorCode, comunity, ba, pos) )
		return false;
	setComunity(comunity);

	if( !ASN1::Encoder::decodePDURequest(mErrorCode, ba, pos) )
		return false;

	// Request ID.
	if( !ASN1::Encoder::decodeInteger(mErrorCode, mRequestID, ba, pos, true) )
		return false;

	// Error code.
	int errorCode;
	// TODO: Shall splitout decoding error code and remote error code?
	if( !ASN1::Encoder::decodeInteger(mErrorCode, errorCode, ba, pos, false) )
		return false;
	mErrorCode = static_cast<ASN1::Encoder::ErrorCode>(errorCode);

	// Error index.
	if( !ASN1::Encoder::decodeInteger(mErrorCode, mErrorObjectIndex, ba, pos, false) )
		return false;

	Int64 varbindListLength;
	if( !ASN1::Encoder::decodeSequence(mErrorCode, ba, pos, varbindListLength) )
		return false;

	// And the variable list.
	PDUVarbind pduVar;
	mVarbindList.clear();
	while( pos < ba.count() )
	{
		Int64 varbindLength;
		if( !ASN1::Encoder::decodeSequence(mErrorCode, ba, pos, varbindLength) )
			return false;
		if( !ASN1::Encoder::decodeObjectIdentifier(mErrorCode, pduVar.oid(), ba, pos) )
			return false;
		if( !ASN1::Encoder::decodeUnknown(mErrorCode, ba, pos, pduVar, includeRawData ? &pduVar.rawValue() : nullptr) )
			return false;
		mVarbindList.append(pduVar);
	}

	// Now, pos shall point to the next data.

	return mErrorCode == ASN1::Encoder::ErrorCode::NoError;
}

StdByteVector Encoder::encodeRequest() const
{
	StdByteVector varbindEncoded;
	StdByteVectorList v;

	for( const PDUVarbind &varbind : mVarbindList )
	{
		varbindEncoded += ASN1::Encoder::encodeSequence(StdByteVectorList()
													 << ASN1::Encoder::encodeObjectIdentifier(varbind.oid())
													 << ASN1::Encoder::encodeUnknown(varbind) );
	}
	return ASN1::Encoder::encodeSequence( StdByteVectorList()
										<< ASN1::Encoder::encodeInteger(mVersion, true)
										<< ASN1::Encoder::encodeOctetString(mComunity)
										<< ASN1::Encoder::encodeList(mRequestType,
																  StdByteVectorList()
																	<< ASN1::Encoder::encodeInteger(mRequestID, false)	// RequestID
																	<< ASN1::Encoder::encodeInteger(0, true)	// Error Code
																	<< ASN1::Encoder::encodeInteger(0, true)	// Error Index
																	<< ASN1::Encoder::encodeSequence(StdByteVectorList() << varbindEncoded) ) );	// Varbind List
}

} // namespace SNMP
