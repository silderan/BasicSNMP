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

#include "stdcharvector.h"
#include "stddeque.h"
#include "stdstring.h"

#include "asn1types.h"
#include "asn1variable.h"


namespace SNMP {

typedef StdDeque<StdByteVector> StdByteVectorList;

class ASN1Encoder
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
	static StdString printableErrorCode(ErrorCode errorCode);

private:
	static bool checkDataType(ErrorCode &errorCode, ASN1DataType actualType, ASN1DataType validType);
	static bool checkDataType(ErrorCode &errorCode, ASN1DataType actualType, const StdVector<ASN1DataType> &validTypes);
	static bool checkDataLength(ErrorCode &errorCode, const StdByteVector &ba, Int64 dataPos, Int64 actualLength, Int64 minLength, Int64 maxLength = 0xFFFFFFFF);
	static bool checkDataRoom(ErrorCode &errorCode, const StdByteVector &ba, Int64 initialDataPos, Int64 dataLength);
	static bool getTLVData(ErrorCode &errorCode, const StdByteVector &ba, Int64 &initialPos, const std::vector<ASN1DataType> &validTypeList, ASN1DataType &actualType, Int64 &length, Int64 minDataLength, Int64 maxDataLength);
	static bool getTLVData(ErrorCode &errorCode, const StdByteVector &ba, Int64 &initialPos, ASN1DataType validType, Int64 &length, Int64 minDataLength, Int64 maxDataLength = 0x7FFFFFFF);
	static bool getTLVData(ErrorCode &errorCode, const StdByteVector &ba, Int64 &initialPos, ASN1DataType &validType, Int64 &length);

	template<typename T>
	static inline bool decodeInteger( ErrorCode &errorCode,
									  T &value,
									  const StdByteVector &ba,
									  Int64 &pos,
									  Int64 length,
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
	static bool decodeInteger(ErrorCode &errorCode, ASN1Variable &asnVar, const StdByteVector &ba, Int64 &pos, Int64 length);

	static bool decodeGenericString(ErrorCode &errorCode, StdByteVector &value, const StdByteVector &ba, Int64 &pos, Int64 length);
	static bool decodeObjectIdentifier(ErrorCode &errorCode, OID &oid, const StdByteVector &ba, Int64 &pos, Int64 length);
	static bool decodeNULL(ErrorCode &errorCode, const StdByteVector &ba, Int64 &pos, Int64 length);

	static bool getLength(ErrorCode &errorCode, const StdByteVector &ba, Int64 &pos, Int64 &length);
	static void setLength(StdByteVector &ba, Int64 pos, Int64 length);

public:
	template<typename T>
	static inline StdByteVector encodeInteger(T value, ASN1DataType type, bool isUnsigned)
	{
		StdByteVector ba;
		ba.append( type );
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
		UInt64 mask = static_cast<UInt64>(0x1FF) << ((8 * (sizeof(T) - 1)) - 1);
		UInt32 integerSize = sizeof(value);

		// Let's lookup for the 9 most significant bits if are all 0 or 1.
		// As could be de value 0xFF00, the use of 0xFF8 mask is necessary.
		while( (integerSize > 1) &&
			   (((static_cast<UInt64>(value) & mask) == 0) ||		// All 0
				(((static_cast<UInt64>(value) & mask) == mask) && !isUnsigned) ))	// All 1
		{
			integerSize--;
			value <<= 8;	// Eliminates the 8 most significant bits.
		}
		setLength( ba, 1, integerSize + (isUnsigned ? 1 : 0) );
		if( isUnsigned )
			ba.append( static_cast<char>(0x00) );
		// Mask 8 first bits: 0xFF00......
		mask = static_cast<UInt64>(0xFF) << (8 * (sizeof(T) -1));
		while( integerSize-- )
		{
			ba.append( static_cast<char>((static_cast<UInt64>(value) & mask) >> (8*(sizeof(T)-1))) );
			value <<= 8;
		}

		return ba;
	}
	static inline bool decodeInteger(ErrorCode &errorCode, ASN1Variable &asnVar, const StdByteVector &ba, Int64 &pos)
	{
		Int64 length;

		static std::vector<ASN1DataType> validNumberTypes = {
			ASN1TYPE_INTEGER,
			ASN1TYPE_Counter,
			ASN1TYPE_Counter64,
			ASN1TYPE_Integer64,
			ASN1TYPE_Gauge32,
			ASN1TYPE_TimeTicks,
			ASN1TYPE_Unsigned64
		};
		ASN1DataType asn1Type;
		if( !getTLVData(errorCode, ba, pos, validNumberTypes, asn1Type, length, 1, asnVar.maxNumberSize()+1) )
			return false;
		asnVar.setType(asn1Type);
		return decodeInteger(errorCode, asnVar, ba, pos, length);
	}
	template<typename T>
	static inline bool decodeInteger(ErrorCode &errorCode, T &number, const StdByteVector &ba, Int64 &pos, bool isUnsigned)
	{
		Int64 length;
		static std::vector<ASN1DataType> validNumberTypes = {
			ASN1TYPE_INTEGER,
			ASN1TYPE_Counter,
			ASN1TYPE_Counter64,
			ASN1TYPE_Integer64,
			ASN1TYPE_Gauge32,
			ASN1TYPE_TimeTicks,
			ASN1TYPE_Unsigned64
		};
		ASN1DataType asn1Type;
		if( !getTLVData(errorCode, ba, pos, validNumberTypes, asn1Type, length, 1, ASN1Variable::maxNumberSize() + (isUnsigned ? 1 : 0)) )
			return false;
		return decodeInteger(errorCode, number, ba, pos, length, isUnsigned);
	}

	static bool decodeGenericString(ErrorCode &errorCode, StdByteVector &value, const StdByteVector &ba, Int64 &pos);
	static bool decodeOctetString(ErrorCode &errorCode, StdByteVector &value, const StdByteVector &ba, Int64 &pos);
	static StdByteVector encodeOctetString(const StdByteVector &value);
	static StdByteVector encodeOctetString(const StdString &value);

	static bool decodeObjectIdentifierValue(ErrorCode &errorCode, const StdByteVector &ba, Int64 &pos, OIDValue &oidValue);
	static void encodeObjectIdentifierValue(const OIDValue &oidValue, StdByteVector &ba);

	static bool decodeObjectIdentifier(ErrorCode &errorCode, OID &oid, const StdByteVector &ba, Int64 &pos);
	static bool decodeObjectIdentifier(ErrorCode &errorCode, ASN1Variable &asn1Var, const StdByteVector &ba, Int64 &pos);
	static StdByteVector encodeObjectIdentifier(const OID &oid);

	static bool decodeNULL(ErrorCode &errorCode, const StdByteVector &ba, Int64 &pos);
	static StdByteVector encodeNULL();

	static bool decodeIPv4Address(ErrorCode &errorCode, Utils::IPv4Address &ipv4, const StdByteVector &ba, Int64 &pos, Int64 length);
	static bool decodeIPv4Address(ErrorCode &errorCode, Utils::IPv4Address &ipv4, const StdByteVector &ba, Int64 &pos);
	static bool decodeIPv4Address(ErrorCode &errorCode, ASN1Variable &asn1Var, const StdByteVector &ba, Int64 &pos);
	static StdByteVector encodeIPv4Address(const StdString &ipv4Address);
	static StdByteVector encodeIPv4Address(const Utils::IPv4Address &ipv4Address);

	static bool decodeUnknown(ErrorCode &errorCode, const StdByteVector &ba, Int64 &pos, Int64 length, ASN1DataType type, ASN1Variable &data);
	static bool decodeUnknown(ErrorCode &errorCode, const StdByteVector &ba, Int64 &pos, ASN1Variable &asn1Var, StdByteVector *rawData);
	static StdByteVector encodeUnknown(const ASN1Variable &pduVariable);

	static bool decodeComplex(ErrorCode &errorCode, const StdByteVector &ba, Int64 &pos, Int64 &length, ASN1DataType &code, StdByteVectorList &baList );
	static StdByteVector encodeList(ASN1DataType asn1Type, const StdByteVectorList &baList );

	static bool decodeSequence(ErrorCode &errorCode, const StdByteVector &ba, Int64 &pos, Int64 &length);
	static StdByteVector encodeSequence(const StdByteVectorList &baList);

	static bool decodePDURequest(ErrorCode &errorCode, const StdByteVector &ba, Int64 &pos);
};

}	// namespace ASN1

#endif // ASN1ENCODER_H
