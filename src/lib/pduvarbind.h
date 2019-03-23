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

#include "stdlist.h"

#include "asn1variable.h"
namespace SNMP {

class PDUVarbind : public ASN1::Variable
{
	OID mOID;
	StdByteVector mRawValue;

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
	const StdByteVector &rawValue()const	{ return mRawValue;	}
	StdByteVector &rawValue()				{ return mRawValue;	}

	const OID &oid()const	{ return mOID;	}
	OID &oid()				{ return mOID;	}
};



typedef StdList<PDUVarbind> PDUVarbindList;


}	// end namespace SNMP

#endif // PDUVARBIND_H
