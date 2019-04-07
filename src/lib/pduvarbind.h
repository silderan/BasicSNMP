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

#ifndef PDUVARBIND_H
#define PDUVARBIND_H

#include "stddeque.h"

#include "asn1variable.h"
namespace SNMP {

class PDUVarbind
{
	ASN1Variable mASN1Var;
	StdByteVector mRawValue;
	OID mOID;

public:
	PDUVarbind( const OID &oid = OID(""), const SNMP::ASN1Variable &asn1Var = SNMP::ASN1Variable() )
		: mASN1Var(asn1Var)
		, mOID(oid)
	{

	}
	PDUVarbind( const PDUVarbind &varbind )
		: mASN1Var(varbind.mASN1Var)
		, mRawValue(varbind.mRawValue)
		, mOID(varbind.mOID)
	{

	}
	void clear()
	{
		mASN1Var.clear();
		mRawValue.clear();
	}
	const StdByteVector &rawValue() const			{ return mRawValue;	}
	StdByteVector &rawValue()						{ return mRawValue;	}
	void setRawValue(const StdByteVector &rawVal)	{ mRawValue = rawVal;	}

	const OID &oid() const			{ return mOID;	}
	OID &oid()						{ return mOID;	}
	void setOID(const OID &oid)		{ mOID = oid;	}

	const ASN1Variable &asn1Variable() const			{ return mASN1Var;	}
	ASN1Variable &asn1Variable()						{ return mASN1Var;	}
	void setASN1Variable(const ASN1Variable &asn1Var)	{ mASN1Var = asn1Var;	}
};



typedef StdDeque<PDUVarbind> PDUVarbindList;


}	// end namespace SNMP

#endif // PDUVARBIND_H
