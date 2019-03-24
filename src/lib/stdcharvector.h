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

#ifndef STDCHARVECTOR_H
#define STDCHARVECTOR_H

#include <string>
#include <cstring>

#include "basic_types.h"
#include "stdstring.h"
#include "stdvector.h"
#include "../utils.h"
namespace SNMP {


class StdByteVector : public StdVector<Byte>
{

public:
	StdByteVector() = default;

	StdByteVector(const StdVector<Byte> &v)
		: StdVector<Byte> (v)
	{	}

	StdByteVector(const char *a, Int64 len)
		: StdByteVector(len)
	{
		while( len-- )
			at(len) = static_cast<Byte>(a[len]);
	}
	StdByteVector(const StdString &s)
		: StdByteVector(s.data(), static_cast<Int64>(s.size()) )
	{	}
	StdByteVector(Int64 t)
		: StdVector<Byte>( static_cast<UInt64>(t) )
	{	}
	Byte &at(Int64 i)
	{
		return StdVector<Byte>::at(i) ;
	}
	Byte at(Int64 i) const
	{
		return StdVector<Byte>::at(i) ;
	}
	Byte operator[](Int64 i) const	{ return at(i);	}
	Byte &operator[](Int64 i)		{ return at(i);	}
	bool isEmpty() const	{ return count() == 0;	}

	bool contains(char c) const
	{
		return Utils::contains(*this, c);
	}
	Int64 count() const		{ return static_cast<Int64>(size());	}
	void resize(Int64 s)	{ StdVector<Byte>::resize(s);			}
	void insert(Int64 pos, Byte c)
	{
		StdVector<Byte>::insert( std::begin(*this) + static_cast<StdByteVector::difference_type>(pos), c );
	}
	void insert(Int64 pos, const StdByteVector &v)
	{
		StdVector<Byte>::insert( std::begin(*this)+pos, std::begin(v), std::end(v) );
	}
	void insert(Int64 pos, const char *c)
	{
		insert( pos, StdByteVector(c, static_cast<Int64>(strlen(c))) );
	}
	StdByteVector mid( Int64 pos, Int64 length ) const
	{
		if( (pos + length) > count() )
			length = static_cast<UInt32>(count() - pos);
		StdByteVector rtn(length);
		while( length-- > 0 )
			rtn[length] = at(pos+length);

		return rtn;
	}
	StdByteVector &append(Byte c)
	{
		push_back(c);
		return *this;
	}

	StdByteVector &append(const StdByteVector &b)
	{
		static_cast<StdVector<Byte> &>(*this).insert( std::end(*this), std::begin(b), std::end(b) );
		return *this;
	}

	StdByteVector &append(const char *b)	{ return append( StdByteVector(b, static_cast<Int64>(strlen(b))) ); }
	StdByteVector &append(const Byte *b)	{ return append( StdByteVector( reinterpret_cast<const char*>(b)) ); }

	StdByteVector &operator +=(const StdByteVector &b) { return append(b); }

	const Byte *bytes() const	{ return StdVector<Byte>::data(); }
	Byte *bytes()				{ return StdVector<Byte>::data(); }

	const char *chars() const	{ return reinterpret_cast<const char*>(bytes());	}
	char *chars()				{ return reinterpret_cast<char*>(bytes());			}

	StdString toStdString() const { return StdString( chars(), size() ); }
};

} // namespace SNMP

#endif // STDCHARVECTOR_H
