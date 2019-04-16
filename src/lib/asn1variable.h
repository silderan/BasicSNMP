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
#include "../utils.h"
#include "oid.h"

namespace SNMP {

class ASN1Variable
{
	ASN1DataType mDataType;

	struct Value
	{
		union Number
		{
			bool boolean;
			int Integer32;
			UInt32 UInteger32;
			Int64 Integer64;
			UInt64 UInteger64;
			float Float;
			double Double;
			Number()
				: UInteger64(0)
			{	}
		}number;
		Utils::IPv4Address ipv4Address;

		bool isNegative;
		OID oid;
		StdByteVector octetString;
		Value()
			: isNegative(false)
		{
		}
	} mDataValue;

public:
	ASN1Variable()
		: mDataType(ASN1TYPE_NULL)
	{
	}
	static int maxNumberSize()			{ return sizeof(double);}
	ASN1DataType type() const			{ return mDataType;		}
	void setType(ASN1DataType asn1Type)	{ mDataType = asn1Type;	}

	void setNull()						{ mDataType = ASN1TYPE_NULL;	}

	bool toBoolean()const				{ return mDataValue.number.boolean;	}
	void setBoolean(bool b)				{ mDataType = ASN1TYPE_BOOLEAN;	mDataValue.number.boolean = b;	}

	Int64 toInteger() const				{ return mDataValue.number.Integer64;	}
	int toInteger32() const				{ return static_cast<int>(mDataValue.number.UInteger64);	}
	void setInteger(Int64 i)			{ mDataType = ASN1TYPE_INTEGER;	mDataValue.number.Integer64 = i;	}

	UInt64 toUInteger() const			{ return mDataValue.number.UInteger64;	}
	UInt32 toUInteger32() const			{ return static_cast<UInt32>(mDataValue.number.UInteger64);	}
	void setUInteger(UInt64 i)			{ mDataType = ASN1TYPE_INTEGER;	mDataValue.number.UInteger64 = i;}

	Int64 toInteger64() const			{ return mDataValue.number.Integer64;	}
	void setInteger64(Int64 i)			{ mDataType = ASN1TYPE_Integer64;	mDataValue.number.Integer64 = i;	}

	UInt64 toUnsigned64() const			{ return mDataValue.number.UInteger64;	}
	void setUnsigned64(UInt64 i)		{ mDataType = ASN1TYPE_Unsigned64;	mDataValue.number.UInteger64 = i;	}

	UInt32 toGauge32() const	{ return toUInteger32();	}
	void setGauge32(UInt32 i)	{ mDataType = ASN1TYPE_Gauge32;	mDataValue.number.UInteger32 = i;	}

	UInt64 toGauge64() const	{ return toUnsigned64();	}
	void setGauge64(UInt64 i)	{ mDataType = ASN1TYPE_Unsigned64;	mDataValue.number.UInteger64 = i;	}

	UInt64 toCounter() const	{ return toUnsigned64();	}
	void setCounter(UInt64 i)	{ mDataType = ASN1TYPE_Counter;	mDataValue.number.UInteger64 = i;	}

	UInt64 toCounter64() const	{ return toUnsigned64();	}
	void setCounter64(UInt64 i)	{ mDataType = ASN1TYPE_Counter64;	mDataValue.number.UInteger64 = i;	}

	UInt64 toTimeTicks() const	{ return toUnsigned64();	}
	void setTimeTicks(UInt64 i)	{ mDataType = ASN1TYPE_TimeTicks;	mDataValue.number.UInteger64 = i;	}

	const OID &toOID() const		{ return mDataValue.oid;	}
	void setOID(const OID &oid)		{ mDataType = ASN1TYPE_OBJECTID;	mDataValue.oid = oid;	}

	// Don't use it to print text as OctetString may not contains the \0 ending. Use toStdString instead.
	const StdByteVector &toOctetString() const		{ return mDataValue.octetString;	}
	void setOctetString(const StdByteVector &str)	{ mDataType = ASN1TYPE_OCTETSTRING;	mDataValue.octetString = str;	}

	StdString toStdString() const					{ return mDataValue.octetString.toStdString(); }

	void setData(ASN1DataType type, const StdByteVector &ba)	{ mDataType = type; mDataValue.octetString = ba;	}

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
