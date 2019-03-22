#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <algorithm>
#include <map>

#include "lib/stdvector.h"

namespace Utils
{
	class IPv4Address
	{
		union
		{
			unsigned int ipNumber;
			struct
			{
				unsigned char octetD;
				unsigned char octetC;
				unsigned char octetB;
				unsigned char octetA;
			}octets;
		}ipv4;
	public:
		IPv4Address(unsigned int ipv4Number = 0)
		{
			ipv4.ipNumber = ipv4Number;
		}
		IPv4Address(unsigned char A, unsigned char B, unsigned char C, unsigned char D)
		{
			ipv4.octets.octetA = A;
			ipv4.octets.octetB = B;
			ipv4.octets.octetC = C;
			ipv4.octets.octetD = D;
		}
		unsigned int number() const				{ return ipv4.ipNumber;			}
		void setNumber(unsigned int ipv4Number)	{ ipv4.ipNumber = ipv4Number;	}

		unsigned char octetA() const    { return ipv4.octets.octetA;    }
		void setOctetA(unsigned char a)	{ ipv4.octets.octetA = a;		}

		unsigned char octetB() const    { return ipv4.octets.octetB;    }
		void setOctetB(unsigned char b)	{ ipv4.octets.octetB = b;		}

		unsigned char octetC() const    { return ipv4.octets.octetC;    }
		void setOctetC(unsigned char c)	{ ipv4.octets.octetC = c;		}

		unsigned char octetD() const    { return ipv4.octets.octetD;    }
		void setOctetD(unsigned char d)	{ ipv4.octets.octetD = d;		}
	};

	template <typename T, typename C>
	StdVector<T> split(const T& s, C separator, bool skipEmptyValues = true)
	{
		std::vector<T> output;

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
	std::string printableByte(T num, unsigned int size = 0)
	{
		if( size <= 0 )
			size = sizeof(T)<<1;

		std::string rtn( size--, '0' );

		unsigned int i = 0;
		do
		{
			rtn[i++] = _hexChar( (num >> (4 * size)) & 0xF );
		}
		while( size-- > 0 );

		return rtn;
	}

	template <typename T>
	std::string printableBytes(const T &vector, const std::string &sep = "")
	{
		typename T::size_type vectorSize = vector.size();
		if( vectorSize == 0 )
			return std::string();
		typename T::size_type separatorSize = sep.size();
		std::string::size_type totalSize = (vectorSize << 1) + (separatorSize * (vectorSize - 1));
		std::string rtn( totalSize, '0' );

		unsigned int vIndex = 0;
		unsigned int outIndex = 0;
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
	inline std::string ipv4AddressToStdString(const IPv4Address &ipv4, const std::string &sep = ".")
	{
		return std::to_string( ipv4.octetA() ) + sep +
			   std::to_string( ipv4.octetB() ) + sep +
			   std::to_string( ipv4.octetC() ) + sep +
			   std::to_string( ipv4.octetD() );
	}
	inline IPv4Address stdStringToIPV4Address(const std::string &stringIP)
	{
		unsigned int ip = 0;
		for( const std::string &num : split(stringIP, '.') )
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
}


#endif // UTILS_H
