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

class PDUVarbind : public ASN1Variable
{
	OID mOID;
	StdByteVector mRawValue;

public:
	PDUVarbind( const OID &oid = "", const SNMP::ASN1Variable &asn1Var = SNMP::ASN1Variable() )
		: SNMP::ASN1Variable(asn1Var)
		, mOID(oid)
	{

	}
	PDUVarbind( const PDUVarbind &varbind )
		: SNMP::ASN1Variable(varbind)
		, mOID(varbind.mOID)
		, mRawValue(varbind.mRawValue)
	{

	}
	void clear()
	{
		SNMP::ASN1Variable::clear();
		mRawValue.clear();
	}
	const StdByteVector &rawValue()const	{ return mRawValue;	}
	StdByteVector &rawValue()				{ return mRawValue;	}

	const OID &oid()const	{ return mOID;	}
	OID &oid()				{ return mOID;	}
};



typedef StdDeque<PDUVarbind> PDUVarbindList;


}	// end namespace SNMP

#endif // PDUVARBIND_H
