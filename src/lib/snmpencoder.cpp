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

void Encoder::setupRequest(int version, const std::string &comunity, int requestID, ASN1::DataType requestCode, const PDUVarbindList &varbindList)
{
	Q_ASSERT( (requestCode == ASN1TYPE_GetRequestPDU) || (requestCode == ASN1TYPE_GetNextRequestPDU) || (requestCode == ASN1TYPE_SetRequestPDU));

	mVersion = version;
	mComunity = comunity;
	mRequestID = requestID;
	mErrorCode = ASN1::Encoder::ErrorCode::NoError;
	mErrorObjectIndex = 0;
	mRequestType = requestCode;
	mVarbindList = varbindList;
}

void Encoder::setupRequest(int version, const std::string &comunity, int requestID, ASN1::DataType requestCode, const OIDList &oidList)
{
	mVarbindList.clear();

	for( const OID &oid : oidList )
		mVarbindList.append( PDUVarbind(oid, ASN1::Variable()) );

	return setupRequest( version, comunity, requestID, requestCode, mVarbindList );
}

void Encoder::setupGetRequest(int version, const std::string &comunity, int requestID, const OID &oid)
{
	return setupRequest(version, comunity, requestID, ASN1TYPE_GetRequestPDU, OIDList(oid) );
}

void Encoder::setupGetRequest(int version, const std::string &comunity, int requestID, const OIDList &oidList)
{
	return setupRequest(version, comunity, requestID, ASN1TYPE_GetRequestPDU, oidList);
}

void Encoder::setupGetNextRequest(int version, const std::string &comunity, int requestID, const OID &oid)
{
	return setupRequest(version, comunity, requestID, ASN1TYPE_GetNextRequestPDU, OIDList(oid));
}

void Encoder::setupGetNextRequest(int version, const std::string &comunity, int requestID, const OIDList &oidList)
{
	return setupRequest(version, comunity, requestID, ASN1TYPE_GetNextRequestPDU, oidList);
}

void Encoder::setupSetRequest(int version, const std::string &comunity, int requestID, const PDUVarbindList &varbindList)
{
	return setupRequest(version, comunity, requestID, ASN1TYPE_SetRequestPDU, varbindList);
}

void Encoder::setupSetRequest(int version, const std::string &comunity, int requestID, const OID &oid, const ASN1::Variable &asn1Var)
{
	return setupSetRequest(version, comunity, requestID, PDUVarbindList() << PDUVarbind(oid, asn1Var) );
}

bool Encoder::decodeAll(const StdCharVector &ba, bool includeRawData)
{
	ASN1::Variable asn1Var;
	long long pos = 0;
	long long length;

	mErrorCode = ASN1::Encoder::ErrorCode::NoError;
	mErrorObjectIndex = 0;
	if( !ASN1::Encoder::decodeSequence(mErrorCode, ba, pos, length) )
		return false;
	if( !ASN1::Encoder::decodeInteger(mErrorCode, mVersion, ba, pos, true) )
		return false;

	// Comunity.
	StdCharVector comunity;
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

	long long varbindListLength;
	if( !ASN1::Encoder::decodeSequence(mErrorCode, ba, pos, varbindListLength) )
		return false;

	// And the variable list.
	PDUVarbind pduVar;
	mVarbindList.clear();
	while( pos < ba.count() )
	{
		long long varbindLength;
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

StdCharVector Encoder::encodeRequest() const
{
	StdCharVector varbindEncoded;
	StdCharVectorList v;

	for( const PDUVarbind &varbind : mVarbindList )
	{
		varbindEncoded += ASN1::Encoder::encodeSequence(StdCharVectorList()
													 << ASN1::Encoder::encodeObjectIdentifier(varbind.oid())
													 << ASN1::Encoder::encodeUnknown(varbind) );
	}
	return ASN1::Encoder::encodeSequence( StdCharVectorList()
										<< ASN1::Encoder::encodeInteger(mVersion, true)
										<< ASN1::Encoder::encodeOctetString(mComunity)
										<< ASN1::Encoder::encodeList(mRequestType,
																  StdCharVectorList()
																	<< ASN1::Encoder::encodeInteger(mRequestID, false)	// RequestID
																	<< ASN1::Encoder::encodeInteger(0, true)	// Error Code
																	<< ASN1::Encoder::encodeInteger(0, true)	// Error Index
																	<< ASN1::Encoder::encodeSequence(StdCharVectorList() << varbindEncoded) ) );	// Varbind List
}

} // namespace SNMP
