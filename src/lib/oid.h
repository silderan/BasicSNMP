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

#ifndef OID_H
#define OID_H

#include "../utils.h"
#include "types.h"

namespace SNMP {

class OIDValue
{
	StdString mStringValue;
	UInt64 mValue;

public:
	OIDValue()
	    : mValue(0)
	{	}
	explicit OIDValue(const char *v)
		: mStringValue( v )
		, mValue( std::stoull(v) )
	{	}
	explicit OIDValue(const StdString &s)
		: OIDValue( s.data() )
	{	}
	template <typename T>
	explicit OIDValue(T v)
		: mValue( static_cast<UInt64>(v) )
	{	}

	StdString toStdString() const
	{
		if( mStringValue.length() == 0 )
			const_cast<OIDValue&>(*this).mStringValue = std::to_string(mValue);
		return mStringValue;
	}
	UInt64 toULongLong() const
	{
		return mValue;
	}
	template <typename T>
	bool operator ==(T v) const	{ return mValue == static_cast<UInt64>(v);	}
	template <typename T>
	bool operator !=(T v) const	{ return mValue != static_cast<UInt64>(v);	}
	bool operator ==(const StdString &v) const	{ return std::stoull(v) == mValue;	}
	bool operator !=(const StdString &v) const	{ return std::stoull(v) != mValue;	}
	bool operator ==(const OIDValue &v) const		{ return v.mValue == mValue;		}
	bool operator !=(const OIDValue &v) const		{ return v.mValue != mValue;		}
};

class OID : public StdVector<OIDValue>
{
public:
	OID() = default;
	OID( Int64 initialSize )
		: StdVector(initialSize)
	{	}
	OID( int initialSize )
		: StdVector(initialSize)
	{	}
	OID( const StdVector<StdString> &oid )
		: StdVector<OIDValue>(oid.count())
	{
		Int64 i = 0;
		for( StdString bit : oid )
			at(i++) = OIDValue(bit);
	}
	OID( const char *charStr )
		: OID( Utils::split( StdString(charStr), '.') )
	{	}
	OID( const StdString &s )
		: OID( Utils::split(s, '.') )
	{	}
	OID( const OIDValue &a )
		: OID(1ll)
	{
		at(0) = a;
	}
	bool startsWith(const OID &oid) const
	{
		if( oid.count() > count() )
			return false;
		for( int i = 0; i < oid.count(); ++i )
			if( oid.at(i) != at(i) )
				return false;
		return true;
	}
	bool endsWith(const OID &oid) const
	{
		if( oid.count() > count() )
			return false;
		for( int i = 1; i <= oid.count(); ++i )
			if( oid.at(oid.count()-i) != at(count()-i) )
				return false;
		return true;
	}
	StdString toStdString() const
	{
		StdString rtn;
		for( OIDValue val : *this )
			rtn += "." +  val.toStdString();
		return rtn;
	}
};
typedef StdVector<OID> OIDList;

} // namespace SNMP

#endif // OID_H
