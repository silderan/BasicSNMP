#ifndef PDUVARBIND_H
#define PDUVARBIND_H

#ifdef QT_CORE_LIB
#include <QString>
#include <QByteArray>
#endif

#include "stdlist.h"

#include "asn1variable.h"
namespace SNMP {

class PDUVarbind : public ASN1::Variable
{
	OID mOID;
	StdCharVector mRawValue;

public:
	PDUVarbind( const OID &oid = "", const ASN1::Variable &asn1Var = ASN1::Variable() )
		: ASN1::Variable(asn1Var)
		, mOID(oid)
	{

	}
	PDUVarbind( const PDUVarbind &varbind )
		: ASN1::Variable(varbind)
		, mOID(varbind.mOID)
		, mRawValue(varbind.mRawValue)
	{

	}
	void clear()
	{
		ASN1::Variable::clear();
		mRawValue.clear();
	}
	PDUVarbind &operator=(const PDUVarbind &other)
	{
		mOID = other.mOID;
		mRawValue = other.mRawValue;
		return *this;
	}
	const StdCharVector &rawValue()const	{ return mRawValue;	}
	StdCharVector &rawValue()				{ return mRawValue;	}

	const OID &oid()const	{ return mOID;	}
	OID &oid()				{ return mOID;	}
};



typedef StdList<PDUVarbind> PDUVarbindList;


}	// end namespace SNMP

#endif // PDUVARBIND_H
