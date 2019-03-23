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

#ifndef ASN1ENCODER_H
#define ASN1ENCODER_H

#include <string>

#include "stdcharvector.h"
#include "stdlist.h"

#include "asn1types.h"
#include "asn1variable.h"

typedef StdList<StdCharVector> StdCharVectorList;

namespace ASN1 {

class Encoder
{
public:
	enum ErrorCode
	{
		DatagramInterrupted = -100,
		UnsignedMalformed,
		NotEnoughRoom,
		NoError = 0,
		TooBig,
		NoSuchName,
		BadValue,
		ReadOnly,
		GenericError,
		NoAccess,
		WrongType,
		WrongLength,
		WrongEncoding,
		WrongValue,
		NoCreation,
		InconsistentValue,
		ResourceUnavailable,
		CommitFailed,
		UndoFailed,
		AuthorizationError,
		NotWritable,
		InconsistentName
	} mErrorCode;
	static std::string printableErrorCode(ErrorCode errorCode);

private:
	static bool checkDataType(ErrorCode &errorCode, ASN1::DataType actualType, ASN1::DataType validType);
	static bool checkDataType(ErrorCode &errorCode, ASN1::DataType actualType, const std::vector<ASN1::DataType> &validTypes);
	static bool checkDataLength(ErrorCode &errorCode, const StdCharVector &ba, long long dataPos, long long actualLength, long long minLength, long long maxLength = 0xFFFFFFFF);
	static bool checkDataRoom(ErrorCode &errorCode, const StdCharVector &ba, long long initialDataPos, long long dataLength);
	static bool getTLVData(ErrorCode &errorCode, const StdCharVector &ba, long long &initialPos, const std::vector<ASN1::DataType> &validTypeList, ASN1::DataType &actualType, long long &length, long long minDataLength, long long maxDataLength);
	static bool getTLVData(ErrorCode &errorCode, const StdCharVector &ba, long long &initialPos, ASN1::DataType validType, long long &length, long long minDataLength, long long maxDataLength = 0x7FFFFFFF);
	static bool getTLVData(ErrorCode &errorCode, const StdCharVector &ba, long long &initialPos, ASN1::DataType &validType, long long &length);

	template<typename T>
	static inline bool decodeInteger( ErrorCode &errorCode,
									  T &value,
									  const StdCharVector &ba,
									  long long &pos,
									  long long length,
									  bool isUnsigned )
	{
		// The first bit must be 0 allways if a unsigned value is encoded.
		// At encode time, if the most significant bit of the value is 1, another byte with 00 must be added.
		if( ba[pos] & 0x80 )
		{
			if( isUnsigned )
			{
				errorCode = ErrorCode::UnsignedMalformed;
				value = 0;
			}
			else
				value = static_cast<T>(-1);
		}
		else
			value = 0;
		while( length-- > 0 )
		{
			value <<= 8;
			value |= (0xFF) & ba[pos++];
		}
		return true;
	}
	static bool decodeInteger(ErrorCode &errorCode, ASN1::Variable &asnVar, const StdCharVector &ba, long long &pos, long long length);

	static bool decodeGenericString(ErrorCode &errorCode, StdCharVector &value, const StdCharVector &ba, long long &pos, long long length);
	static bool decodeObjectIdentifier(ErrorCode &errorCode, OID &oid, const StdCharVector &ba, long long &pos, long long length);
	static bool decodeNULL(ErrorCode &errorCode, const StdCharVector &ba, long long &pos, long long length);

	static bool getLength(ErrorCode &errorCode, const StdCharVector &ba, long long &pos, long long &length);
	static void setLength(StdCharVector &ba, long long pos, long long length);

public:
	template<typename T>
	static inline StdCharVector encodeInteger(T value, bool isUnsigned)
	{
		StdCharVector ba;
		ba.append( ASN1TYPE_INTEGER );
		// For this small values, below 127, no need to go through such complicated code.
		if( !(value & ~static_cast<T>(0x7F))  )
		{
			ba.append( static_cast<char>(0x01) );
			ba.append( static_cast<char>(value) );
			return ba;
		}

		if( isUnsigned && !(value & static_cast<T>(0x1) << ((8 * sizeof(T)) - 1)) )
			isUnsigned = false;

		// Mask 9 first bits of the value.
		// This will create 0xFF80000... value.
		quint64 mask = static_cast<quint64>(0x1FF) << ((8 * (sizeof(T) - 1)) - 1);
		quint32 integerSize = sizeof(value);

		// Let's lookup for the 9 most significant bits if are all 0 or 1.
		// As could be de value 0xFF00, the use of 0xFF8 mask is necessary.
		while( (integerSize > 1) &&
			   (((static_cast<quint64>(value) & mask) == 0) ||		// All 0
				(((static_cast<quint64>(value) & mask) == mask) && !isUnsigned) ))	// All 1
		{
			integerSize--;
			value <<= 8;	// Eliminates the 8 most significant bits.
		}
		setLength( ba, 1, integerSize + (isUnsigned ? 1 : 0) );
		if( isUnsigned )
			ba.append( static_cast<char>(0x00) );
		// Mask 8 first bits: 0xFF00......
		mask = static_cast<quint64>(0xFF) << (8 * (sizeof(T) -1));
		while( integerSize-- )
		{
			ba.append( static_cast<char>((static_cast<quint64>(value) & mask) >> (8*(sizeof(T)-1))) );
			value <<= 8;
		}

		return ba;
	}
	static inline bool decodeInteger(ErrorCode &errorCode, ASN1::Variable &asnVar, const StdCharVector &ba, long long &pos)
	{
		long long length;

		static std::vector<ASN1::DataType> validNumberTypes = {
			ASN1TYPE_INTEGER,
			ASN1TYPE_Counter,
			ASN1TYPE_Counter64,
			ASN1TYPE_Integer64,
			ASN1TYPE_Gauge,
			ASN1TYPE_TimeTicks,
			ASN1TYPE_Unsigned64
		};
		ASN1::DataType asn1Type;
		if( !getTLVData(errorCode, ba, pos, validNumberTypes, asn1Type, length, 1, asnVar.maxNumberSize()+1) )
			return false;
		asnVar.setType(asn1Type);
		return decodeInteger(errorCode, asnVar, ba, pos, length);
	}
	template<typename T>
	static inline bool decodeInteger(ErrorCode &errorCode, T &number, const StdCharVector &ba, long long &pos, bool isUnsigned)
	{
		long long length;
		static std::vector<ASN1::DataType> validNumberTypes = {
			ASN1TYPE_INTEGER,
			ASN1TYPE_Counter,
			ASN1TYPE_Counter64,
			ASN1TYPE_Integer64,
			ASN1TYPE_Gauge,
			ASN1TYPE_TimeTicks,
			ASN1TYPE_Unsigned64
		};
		ASN1::DataType asn1Type;
		if( !getTLVData(errorCode, ba, pos, validNumberTypes, asn1Type, length, 1, ASN1::Variable::maxNumberSize() + (isUnsigned ? 1 : 0)) )
			return false;
		return decodeInteger(errorCode, number, ba, pos, length, isUnsigned);
	}

	static bool decodeGenericString(ErrorCode &errorCode, StdCharVector &value, const StdCharVector &ba, long long &pos);
	static bool decodeOctetString(ErrorCode &errorCode, StdCharVector &value, const StdCharVector &ba, long long &pos);
	static StdCharVector encodeOctetString(const StdCharVector &value);
	static StdCharVector encodeOctetString(const std::string &value);

	static bool decodeObjectIdentifierValue(ErrorCode &errorCode, const StdCharVector &ba, long long &pos, OIDValue &oidValue);
	static void encodeObjectIdentifierValue(const OIDValue &oidValue, StdCharVector &ba);

	static bool decodeObjectIdentifier(ErrorCode &errorCode, OID &oid, const StdCharVector &ba, long long &pos);
	static bool decodeObjectIdentifier(ErrorCode &errorCode, ASN1::Variable &asn1Var, const StdCharVector &ba, long long &pos);
	static StdCharVector encodeObjectIdentifier(const OID &oid);

	static bool decodeNULL(ErrorCode &errorCode, const StdCharVector &ba, long long &pos);
	static StdCharVector encodeNULL();

	static bool decodeIPv4Address(ErrorCode &errorCode, Utils::IPv4Address &ipv4, const StdCharVector &ba, long long &pos, long long length);
	static bool decodeIPv4Address(ErrorCode &errorCode, Utils::IPv4Address &ipv4, const StdCharVector &ba, long long &pos);
	static bool decodeIPv4Address(ErrorCode &errorCode, ASN1::Variable &asn1Var, const StdCharVector &ba, long long &pos);
	static StdCharVector encodeIPv4Address(const std::string &ipv4Address);
	static StdCharVector encodeIPv4Address(const Utils::IPv4Address &ipv4Address);

	static bool decodeUnknown(ErrorCode &errorCode, const StdCharVector &ba, long long &pos, long long length, ASN1::DataType type, ASN1::Variable &data);
	static bool decodeUnknown(ErrorCode &errorCode, const StdCharVector &ba, long long &pos, ASN1::Variable &asn1Var, StdCharVector *rawData);
	static StdCharVector encodeUnknown(const ASN1::Variable &pduVariable);

	static bool decodeComplex(ErrorCode &errorCode, const StdCharVector &ba, long long &pos, long long &length, ASN1::DataType &code, StdCharVectorList &baList );
	static StdCharVector encodeList(ASN1::DataType asn1Type, const StdCharVectorList &baList );

	static bool decodeSequence(ErrorCode &errorCode, const StdCharVector &ba, long long &pos, long long &length);
	static StdCharVector encodeSequence(const StdCharVectorList &baList);

	static bool decodePDURequest(ErrorCode &errorCode, const StdCharVector &ba, long long &pos);
};

}	// namespace ASN1

#endif // ASN1ENCODER_H
