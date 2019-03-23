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

#ifndef ASN1VARIABLE_H
#define ASN1VARIABLE_H

#include "stdcharvector.h"

#include "asn1types.h"
#include "utils.h"
#include "oid.h"

namespace ASN1 {

class Variable
{
	ASN1::DataType mDataType;

	struct Value
	{
		union Number
		{
			bool boolean;
			int Integer32;
			unsigned int UInteger32;
			long long Integer64;
			unsigned long long UInteger64;
			float Float;
			double Double;
			Number()
				: UInteger64(0)
			{	}
		}number;
		Utils::IPv4Address ipv4Address;

		bool isNegative;
		OID oid;
		StdCharVector octetString;
		Value()
			: isNegative(false)
		{
		}
		~Value()
		{

		}
	} mDataValue;

public:
	Variable()
		: mDataType(ASN1TYPE_NULL)
	{
	}
	Variable( const Variable &pduVar )
		: mDataType(pduVar.mDataType)
	{
		*this = pduVar;
	}
	Variable &operator =(const Variable &pduVar);

	static int maxNumberSize()				{ return sizeof(double);	}
	ASN1::DataType type() const				{ return mDataType;		}
	void setType(ASN1::DataType asn1Type)	{ mDataType = asn1Type;	}

	void setNull()						{ mDataType = ASN1TYPE_NULL;	}

	bool toBoolean()const				{ return mDataValue.number.boolean;	}
	void setBoolean(bool b)				{ mDataType = ASN1TYPE_BOOLEAN;	mDataValue.number.boolean = b;	}

	long long toInteger() const			{ return mDataValue.number.Integer64;	}
	int toInteger32() const				{ return static_cast<int>(mDataValue.number.UInteger64);	}
	void setInteger(long long i)		{ mDataType = ASN1TYPE_INTEGER;	mDataValue.number.Integer64 = i;	}

	unsigned long long toUInteger() const	{ return mDataValue.number.UInteger64;	}
	unsigned int toUInteger32() const		{ return static_cast<unsigned int>(mDataValue.number.UInteger64);	}
	void setUInteger(unsigned long long i)	{ mDataType = ASN1TYPE_INTEGER;	mDataValue.number.UInteger64 = i;}

	long long toInteger64() const			{ return mDataValue.number.Integer64;	}
	void setInteger64(long long i)			{ mDataType = ASN1TYPE_Integer64;	mDataValue.number.Integer64 = i;	}

	unsigned long long toUnsigned64() const			{ return mDataValue.number.UInteger64;	}
	void setUnsigned64(unsigned long long i)		{ mDataType = ASN1TYPE_Unsigned64;	mDataValue.number.UInteger64 = i;	}

	unsigned long long toGauge() const	{ return toUnsigned64();	}
	void setGauge(unsigned long long i)	{ mDataType = ASN1TYPE_Gauge;	mDataValue.number.UInteger64 = i;	}

	unsigned long long toGauge64() const	{ return toUnsigned64();	}
	void setGauge64(unsigned long long i)	{ mDataType = ASN1TYPE_Unsigned64;	mDataValue.number.UInteger64 = i;	}

	unsigned long long toCounter() const	{ return toUnsigned64();	}
	void setCounter(unsigned long long i)	{ mDataType = ASN1TYPE_Counter;	mDataValue.number.UInteger64 = i;	}

	unsigned long long toCounter64() const	{ return toUnsigned64();	}
	void setCounter64(unsigned long long i)	{ mDataType = ASN1TYPE_Counter64;	mDataValue.number.UInteger64 = i;	}

	unsigned long long toTimeTicks() const	{ return toUnsigned64();	}
	void setTimeTicks(unsigned long long i)	{ mDataType = ASN1TYPE_TimeTicks;	mDataValue.number.UInteger64 = i;	}

	const OID &toOID() const		{ return mDataValue.oid;	}
	void setOID(const OID &oid)		{ mDataType = ASN1TYPE_OBJECTID;	mDataValue.oid = oid;	}

	// Don't use it to print text as OctetString may not contains the \0 ending. Use toDisplayString instead.
	const StdCharVector &toOctetString() const		{ return mDataValue.octetString;	}
	void setOctetString(const StdCharVector &str)	{ mDataType = ASN1TYPE_OCTETSTRING;	mDataValue.octetString = str;	}

	std::string toDisplayString() const				{ return std::string(mDataValue.octetString.data(), mDataValue.octetString.size());	}

	void setData(ASN1::DataType type, const StdCharVector &ba)	{ mDataType = type; mDataValue.octetString = ba;	}

	const Utils::IPv4Address &toIPV4() const			{ return mDataValue.ipv4Address;	}
	void setIPv4(const Utils::IPv4Address &ipv4)		{ mDataValue.ipv4Address = ipv4;	}
	void setIPv4(unsigned char octetA, unsigned char octetB, unsigned char octetC, unsigned char octetD)
	{
		mDataValue.ipv4Address.setOctetA( octetA );
		mDataValue.ipv4Address.setOctetB( octetB );
		mDataValue.ipv4Address.setOctetC( octetC );
		mDataValue.ipv4Address.setOctetD( octetD );
	}

	void clear()
	{
		mDataType = ASN1TYPE_NULL;
	}
};
} // namespace ASN1

#endif // ASN1VARIABLE_H
