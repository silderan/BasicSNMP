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

#ifndef UTILS_H
#define UTILS_H

#include <assert.h>
#include <string>
#include <algorithm>
#include <map>

#include "lib/stdvector.h"
#include "lib/stdstring.h"

namespace SNMP {
namespace Utils {

	class IPv4Address
	{
		union
		{
			SNMP::UInt32 ipNumber;
			struct
			{
				Byte octetD;
				Byte octetC;
				Byte octetB;
				Byte octetA;
			}octets;
		}ipv4;
	public:
		IPv4Address(UInt32 ipv4Number = 0)
		{
			ipv4.ipNumber = ipv4Number;
		}
		IPv4Address(Byte A, Byte B, Byte C, Byte D)
		{
			ipv4.octets.octetA = A;
			ipv4.octets.octetB = B;
			ipv4.octets.octetC = C;
			ipv4.octets.octetD = D;
		}
		UInt32 number() const				{ return ipv4.ipNumber;			}
		void setNumber(UInt32 ipv4Number)	{ ipv4.ipNumber = ipv4Number;	}

		Byte octetA() const		{ return ipv4.octets.octetA;    }
		void setOctetA(Byte a)	{ ipv4.octets.octetA = a;		}

		Byte octetB() const		{ return ipv4.octets.octetB;    }
		void setOctetB(Byte b)	{ ipv4.octets.octetB = b;		}

		Byte octetC() const		{ return ipv4.octets.octetC;    }
		void setOctetC(Byte c)	{ ipv4.octets.octetC = c;		}

		Byte octetD() const		{ return ipv4.octets.octetD;    }
		void setOctetD(Byte d)	{ ipv4.octets.octetD = d;		}

		bool operator==(const IPv4Address &other) const
		{
			return ipv4.ipNumber == other.ipv4.ipNumber;
		}
		bool operator!=(const IPv4Address &other) const
		{
			return ipv4.ipNumber != other.ipv4.ipNumber;
		}
	};

	template <typename T, typename C>
	StdVector<T> split(const T& s, C separator, bool skipEmptyValues = true)
	{
		StdVector<T> output;

		typename T::size_type prev_pos = 0, pos = 0;

		while( (pos = s.find(separator, pos)) != T::npos )
		{
			if( !skipEmptyValues || (pos-prev_pos != 0) )
				output.push_back( s.substr(prev_pos, pos-prev_pos) );

			prev_pos = ++pos;
		}

		if( !skipEmptyValues || (s.size()-prev_pos) )
			output.push_back( s.substr(prev_pos, (s.size()-prev_pos)) ); // Last word

		return output;
	}

	inline char _hexChar(char c)
	{
		if( c > 9 )
			return (c - 10) + 'A';
		return c + '0';
	}

	template <typename T>
	StdString printableByte(T num, UInt32 size = 0)
	{
		if( size <= 0 )
			size = sizeof(T)<<1;

		StdString rtn( size--, '0' );

		UInt32 i = 0;
		do
		{
			rtn[i++] = _hexChar( (num >> (4 * size)) & 0xF );
		}
		while( size-- > 0 );

		return rtn;
	}

	template <typename T>
	StdString printableBytes(const T &vector, const StdString &sep = "")
	{
		typename T::size_type vectorSize = vector.size();
		if( vectorSize == 0 )
			return StdString();
		typename T::size_type separatorSize = sep.size();
		StdString::size_type totalSize = (vectorSize << 1) + (separatorSize * (vectorSize - 1));
		StdString rtn( totalSize, '0' );

		UInt32 vIndex = 0;
		UInt32 outIndex = 0;
		do
		{
			rtn[outIndex++] = _hexChar( (vector[vIndex] >> 4) & 0xF );
			rtn[outIndex++] = _hexChar( vector[vIndex++] & 0xF );
			if( (vIndex != vectorSize) && (separatorSize != 0) )
			{
				rtn.replace( outIndex, separatorSize, sep );
				outIndex += separatorSize;
			}
		}
		while( vIndex < vectorSize );

		return rtn;
	}
	inline StdString ipv4AddressToStdString(const IPv4Address &ipv4, const StdString &sep = ".")
	{
		return std::to_string( ipv4.octetA() ) + sep +
			   std::to_string( ipv4.octetB() ) + sep +
			   std::to_string( ipv4.octetC() ) + sep +
			   std::to_string( ipv4.octetD() );
	}
	inline IPv4Address stdStringToIPV4Address(const StdString &stringIP)
	{
		UInt32 ip = 0;
		for( const StdString &num : split(stringIP, '.') )
			ip = (ip << 8) + std::stoul(num);
		return Utils::IPv4Address(ip);
	}

	template <class V, typename C>
	bool contains(const V &vector, const C &item)
	{
		return std::find( vector.begin(), vector.end(), item ) != vector.end();
	}

	template <class K, typename V>
	V value(const std::map<K, V> &m, const K &k, const V &def = V())
	{
		auto it = m.find(k);
		if( it != m.end() )
			return it->second;
		return def;
	}

};	// namespace Utils
};	// namespace SNMP


#endif // UTILS_H
