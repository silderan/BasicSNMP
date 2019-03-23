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

#include "asn1encoder.h"

namespace ASN1 {

bool Encoder::checkDataType(ErrorCode &errorCode, ASN1::DataType actualType, ASN1::DataType validType)
{
	if( actualType != validType )
	{
		errorCode = ErrorCode::WrongType;
		return false;
	}
	return true;
}

bool Encoder::checkDataType(ErrorCode &errorCode, ASN1::DataType actualType, const std::vector<ASN1::DataType> &validTypes)
{
	if( validTypes.size() && !Utils::contains(validTypes, actualType) )
	{
		errorCode = ErrorCode::WrongType;
		return false;
	}
	return true;
}

bool Encoder::checkDataLength(ErrorCode &errorCode, const StdCharVector &ba, long long dataPos, long long actualLength, long long minLength, long long maxLength)
{
	if( actualLength < minLength )
	{
		errorCode = ErrorCode::WrongLength;
	}
	else
	if( actualLength > maxLength )
	{
		errorCode = ErrorCode::WrongLength;
	}
	else
	if( (dataPos + actualLength) > static_cast<quint32>(ba.count()) )
	{
		errorCode = ErrorCode::NotEnoughRoom;
	}
	else
		return true;
	return false;
}

bool Encoder::checkDataRoom(ErrorCode &errorCode, const StdCharVector &ba, long long initialDataPos, long long dataLength)
{
	if( (ba.count() - initialDataPos) < dataLength )
	{
		errorCode = ErrorCode::NotEnoughRoom;
		return false;
	}
	return true;
}

bool Encoder::getTLVData(ErrorCode &errorCode,
							const StdCharVector &ba,		// Byte Array received from SNMP agent.
							long long &initialPos,			// Initial pos in the array to analize. Will point to data position if no error.
							const std::vector<ASN1::DataType> &validTypeList,	// Valid data type.
							ASN1::DataType &actualType,		// Actual data received. Will be set if no error.
							long long &length,				// The length of the data to retrieve. Will be set if no error.
							long long minDataLength,		// The minimim data required. usually, it's 1.
							long long maxDataLength)		// Max data length for data. For example. if a signed 32 integer is spected, data length should be 4. 5 if it's a unsigned integer because of the 00 leading byte needed for this.
{
	if( ba.count() <= initialPos )
	{
		errorCode = ErrorCode::DatagramInterrupted;
		return false;
	}
	actualType = ba[initialPos];
	if( !checkDataType(errorCode, actualType, validTypeList) )
		return false;

	initialPos++;
	if( !getLength(errorCode, ba, initialPos, length) )
		return false;

	return checkDataLength(errorCode, ba, initialPos, length, minDataLength, maxDataLength);
}

bool Encoder::getTLVData(ErrorCode &errorCode,
							const StdCharVector &ba,
							long long &initialPos,
							ASN1::DataType validType,
							long long &length,
							long long minDataLength,
							long long maxDataLength)
{
	if( ba.count() <= initialPos )
	{
		errorCode = ErrorCode::DatagramInterrupted;
		return false;
	}
	if( !checkDataType(errorCode, ba[initialPos], validType) )
		return false;

	initialPos++;
	if( !getLength(errorCode, ba, initialPos, length) )
		return false;

	return checkDataLength(errorCode, ba, initialPos, length, minDataLength, maxDataLength);
}

bool Encoder::getTLVData(ErrorCode &errorCode,
							const StdCharVector &ba,
							long long &initialPos,
							ASN1::DataType &validType,
							long long &length)
{
	if( ba.count() <= initialPos )
	{
		errorCode = ErrorCode::DatagramInterrupted;
		return false;
	}
	validType = ba[initialPos++];

	if( !getLength(errorCode, ba, initialPos, length) )
		return false;

	return checkDataLength(errorCode, ba, initialPos, length, 0, 0xFFFFFFFF);
}

bool Encoder::getLength(ErrorCode &errorCode, const StdCharVector &ba, long long &pos, long long &length)
{
	if( pos < static_cast<quint32>(ba.size()) )
	{
		if( !(ba[pos] & 0x80) ) // Less that 127.
		{
			length = static_cast<quint32>(ba[pos++]);
			return true;
		}
		quint32 count = ba[pos++] & 0x7F;
		// Check if there is enough rooom in the data for all bytes.
		if( (static_cast<quint32>(ba.size()) - pos) >= count )
		{
			length = 0;
			while( count > 0 )
			{
				length <<= 8;
				length += (0xFF & static_cast<quint32>(ba[pos++]));
				--count;
			}
			return true;
		}
	}
	errorCode = ErrorCode::DatagramInterrupted;
	return false;
}

void Encoder::setLength(StdCharVector &ba, long long pos, long long length)
{
	// Length has two forms to be encoded.
	// 1: If value is less than 127, it's encoded directly.
	// 2: If value is greather, first byte encoded in the 7 less significant bits de quantity of bytes used to encode the actual length
	if( length < 127 )
		ba.insert( pos, static_cast<char>(length) );
	else
	{
		int count = static_cast<int>( ((length - 1) >> 8) + 1 );
		ba.insert( pos++, static_cast<char>(count | 0x80) );

		while( length != 0 )
		{
			ba.insert( pos, static_cast<char>(length & 0xFF) );
			length >>= 8;
		}
	}
}

bool Encoder::decodeNULL(ErrorCode &errorCode, const StdCharVector &ba, long long &pos, long long length)
{
	Q_UNUSED(errorCode);
	Q_UNUSED(ba);
	Q_UNUSED(pos);
	Q_UNUSED(length);
	return true;
}

bool Encoder::decodeInteger(ErrorCode &errorCode,
							   ASN1::Variable &asnVar,
							   const StdCharVector &ba,
							   long long &pos,
							   long long length)
{
	if( ba[pos] & 0x80 )
	{
		unsigned long long ui;
		bool rtn = decodeInteger(errorCode, ui, ba, pos, length, true);
		switch( asnVar.type() )
		{
		case ASN1TYPE_INTEGER:		asnVar.setUInteger(ui);		break;
		case ASN1TYPE_Counter:		asnVar.setCounter(ui);		break;
		case ASN1TYPE_Counter64:	asnVar.setCounter64(ui);	break;
		case ASN1TYPE_Integer64:	asnVar.setUnsigned64(ui);	break;
		case ASN1TYPE_Gauge:		asnVar.setGauge(ui);		break;
		case ASN1TYPE_TimeTicks:	asnVar.setTimeTicks(ui);	break;
		case ASN1TYPE_Unsigned64:	asnVar.setUnsigned64(ui);	break;
		}
		return rtn;
	}
	else
	{
		long long i;
		bool rtn = decodeInteger(errorCode, i, ba, pos, length, false);
		asnVar.setInteger(i);
		return rtn;
	}
}




StdCharVector Encoder::encodeOctetString(const StdCharVector &value)
{
	StdCharVector ba;
	ba.append( ASN1TYPE_OCTETSTRING );
	setLength( ba, 1, value.count() );
	ba.append( value.data() );
	return ba;
}

StdCharVector Encoder::encodeOctetString(const std::string &value)
{
	StdCharVector ba;
	ba.append( ASN1TYPE_OCTETSTRING );
	setLength( ba, 1, static_cast<long long>(value.size()) );
	ba.append( value.data() );
	return ba;
}

void Encoder::encodeObjectIdentifierValue(const OIDValue &oidValue, StdCharVector &ba)
{
	int bitShift = 63;
	unsigned long long mask = 0x7F00000000000000u;
	unsigned long long value = oidValue.toULongLong();
	while( mask )
	{
		if( value > mask )
			ba.append( static_cast<char>((value >> bitShift) & 0x7Fu) | 0x80 );
		mask >>= 7;
		bitShift -= 7;
	}
	ba.append( static_cast<char>(value & 0x7Fu) );
}
bool Encoder::decodeObjectIdentifierValue(ErrorCode &errorCode, const StdCharVector &ba, long long &pos, OIDValue &oidValue)
{
	unsigned long long value = 0;
	do
	{
		if( static_cast<quint32>(ba.count()) <= pos )
		{
			errorCode = ErrorCode::NotEnoughRoom;
			return false;
		}
		value <<= 7;
		value += ba[pos] & 0x7F;
	}
	while( ba[pos++] & 0x80 );
	oidValue = value;
	return true;
}

bool Encoder::decodeObjectIdentifier(ErrorCode &errorCode,
										OID &oid,
										const StdCharVector &ba,
										long long &pos,
										long long length)
{
	Q_UNUSED(errorCode);

	OIDValue value;

	oid.clear( );
	// At this point, we don't know the OID lenght, but we can make a guess.
	oid.reserve( static_cast<OID::size_type>(length + 1) );
	length += pos;

	oid.append( OIDValue(ba[pos] / 40) );
	oid.append( OIDValue(ba[pos++] % 40) );

	for( long long i = 2; (length > pos) && decodeObjectIdentifierValue(errorCode, ba, pos, value); ++i )
		oid.append( value );

	return true;
}
StdCharVector Encoder::encodeObjectIdentifier(const OID &oid)
{
	StdCharVector ba;
	Q_ASSERT( oid.count() > 1 );
	ba.append( ASN1TYPE_OBJECTID );

	// First 2 elements have special coding: 40x[0]+[1] in the first byte.
	// That is because both are allways small. 1.3.
	// Bad design, anyway: the x40 math for the first value is a nonsense, IMHO.
	ba.append( static_cast<char>( oid[0].toULongLong() * 40 + oid[1].toULongLong()) );

	// From 3dr element and so on, all are identical.
	for( long long pos = 2; pos < oid.count(); ++pos )
		encodeObjectIdentifierValue( oid[pos], ba );

	setLength( ba, 1, static_cast<quint32>(ba.count()-1) );
	return ba;
}

bool Encoder::decodeObjectIdentifier(ErrorCode &errorCode, OID &oid, const StdCharVector &ba, long long &pos)
{
	long long length;
	if( !getTLVData(errorCode, ba, pos, ASN1TYPE_OBJECTID, length, 1) )
		return false;

	return decodeObjectIdentifier(errorCode, oid, ba, pos, length);
}

bool Encoder::decodeObjectIdentifier(ErrorCode &errorCode, ASN1::Variable &asn1Var, const StdCharVector &ba, long long &pos)
{
	OID oid;
	if( decodeObjectIdentifier(errorCode, oid, ba, pos) )
	{
		asn1Var.setOID(oid);
		return true;
	}
	return false;
}

bool Encoder::decodeGenericString(ErrorCode &errorCode,
									 StdCharVector &value,
									 const StdCharVector &ba,
									 long long &pos,
									 long long length)
{
	Q_UNUSED(errorCode);
	value = ba.mid( pos, length );
	pos += length;
	return true;
}

bool Encoder::decodeGenericString(ErrorCode &errorCode,
									 StdCharVector &value,
									 const StdCharVector &ba,
									 long long &pos)
{
	ASN1::DataType dataType;
	long long length;

	if( !getTLVData(errorCode, ba, pos, dataType, length) )
		return false;

	if( (dataType == ASN1TYPE_OCTETSTRING) ||
		((dataType >= ASN1TYPE_NumericString) && (dataType <= ASN1TYPE_IA5String)) )
	{
		return decodeGenericString(errorCode, value, ba, pos, length);
	}
	errorCode = ErrorCode::WrongType;
	return false;
}

bool Encoder::decodeOctetString(ErrorCode &errorCode, StdCharVector &value, const StdCharVector &ba, long long &pos)
{
	return decodeGenericString(errorCode, value, ba, pos);
}

bool Encoder::decodeNULL(ErrorCode &errorCode, const StdCharVector &ba, long long &pos)
{
	long long length;

	if( !getTLVData(errorCode, ba, pos, ASN1TYPE_NULL, length, 0, 0) )
		return false;

	return decodeNULL(errorCode, ba, pos, length);
}

StdCharVector Encoder::encodeNULL()
{
	StdCharVector ba;
	ba.append( ASN1TYPE_NULL );
	ba.append( static_cast<char>(0) );
	return ba;
}

bool Encoder::decodeIPv4Address(ErrorCode &errorCode, Utils::IPv4Address &ipv4, const StdCharVector &ba, long long &pos)
{
	long long length;
	if( !getTLVData(errorCode, ba, pos, ASN1TYPE_IPv4Address, length, 4, 4) )
		return false;

	ipv4 =	static_cast<unsigned>(static_cast<unsigned char>(ba[pos+0]) << 24) +
			static_cast<unsigned>(static_cast<unsigned char>(ba[pos+1]) << 16) +
			static_cast<unsigned>(static_cast<unsigned char>(ba[pos+2]) << 8) +
			static_cast<unsigned>(static_cast<unsigned char>(ba[pos+3])) ;

	pos+=4;
	return true;
}

bool Encoder::decodeIPv4Address(ErrorCode &errorCode, ASN1::Variable &asn1Var, const StdCharVector &ba, long long &pos)
{
	Utils::IPv4Address ipv4;
	if( decodeIPv4Address(errorCode, ipv4, ba, pos) )
	{
		asn1Var.setIPv4(ipv4);
		return true;
	}
	return false;
}

bool Encoder::decodeIPv4Address(ErrorCode &errorCode, Utils::IPv4Address &ipv4, const StdCharVector &ba, long long &pos, long long length)
{
	if( length != 4 )
	{
		errorCode = ErrorCode::WrongLength;
		return false;
	}
	ipv4 =	static_cast<unsigned>(static_cast<unsigned char>(ba[pos+0]) << 24) +
			static_cast<unsigned>(static_cast<unsigned char>(ba[pos+1]) << 16) +
			static_cast<unsigned>(static_cast<unsigned char>(ba[pos+2]) << 8) +
			static_cast<unsigned>(static_cast<unsigned char>(ba[pos+3])) ;

	pos+=4;
	return true;
}

StdCharVector Encoder::encodeIPv4Address(const std::string &ipv4Address)
{
	StdCharVector ba;
	ba.append( ASN1TYPE_IPv4Address );
	setLength( ba, 1, 4 );

	for( const std::string &num : Utils::split(ipv4Address, '.') )
		ba.append( static_cast<char>(0xFF & std::stoul(num)) );

	return ba;
}

StdCharVector Encoder::encodeIPv4Address(const Utils::IPv4Address &ipv4Address)
{
	StdCharVector ba;
	ba.append( ASN1TYPE_IPv4Address );
	setLength( ba, 1, 4 );

	ba.append( static_cast<char>(ipv4Address.octetA()) );
	ba.append( static_cast<char>(ipv4Address.octetB()) );
	ba.append( static_cast<char>(ipv4Address.octetC()) );
	ba.append( static_cast<char>(ipv4Address.octetD()) );
	return ba;
}

bool Encoder::decodeUnknown(ErrorCode &errorCode,
								const StdCharVector &ba,
								long long &pos,
								long long length,
								ASN1::DataType asn1Type,
								ASN1::Variable &data )
{
	bool rtn;
	switch( asn1Type )
	{
	case ASN1TYPE_NULL:
		rtn = decodeNULL(errorCode, ba, pos, length);
		data.setNull();
		break;
	case ASN1TYPE_INTEGER:
	case ASN1TYPE_Gauge:
	case ASN1TYPE_Counter:
	case ASN1TYPE_Counter64:
	case ASN1TYPE_Integer64:
	case ASN1TYPE_Unsigned64:
		rtn = decodeInteger(errorCode, data, ba, pos, length);
		break;
	case ASN1TYPE_OBJECTID:
		{
			OID oid;
			rtn = decodeObjectIdentifier(errorCode, oid, ba, pos, length);
			data.setOID(oid);
		}
		break;
	case ASN1TYPE_OCTETSTRING:
	  {
		StdCharVector s;
		rtn = decodeGenericString( errorCode, s, ba, pos, length );
		data.setOctetString(s);
	  }
		break;
	case ASN1TYPE_IPv4Address:
	  {
		Utils::IPv4Address ipv4;
		rtn = decodeIPv4Address( errorCode, ipv4, ba, pos, length );
		data.setIPv4(ipv4);
	  }
		break;
	case ASN1TYPE_Opaque:
	default:
		{
			rtn = true;
			data.setData( asn1Type, ba.mid(pos, length) );
			pos+=length;
		}
		break;
	}
	return rtn;
}

bool Encoder::decodeUnknown(ErrorCode &errorCode, const StdCharVector &ba, long long &pos, ASN1::Variable &asn1Var, StdCharVector *rawData)
{
	long long length;
	ASN1::DataType asn1Type;
	if( !getTLVData(errorCode, ba, pos, asn1Type, length) )
		return false;

	if( rawData != Q_NULLPTR )
		*rawData = ba.mid( pos, length );

	return decodeUnknown(errorCode, ba, pos, length, asn1Type, asn1Var);
}

StdCharVector Encoder::encodeUnknown( const ASN1::Variable &pduVariable )
{
	switch( pduVariable.type() )
	{
	case ASN1TYPE_NULL:			return encodeNULL();
	case ASN1TYPE_INTEGER:		return encodeInteger( pduVariable.toInteger(), false );
	case ASN1TYPE_Gauge:		return encodeInteger( pduVariable.toGauge(), true );
	case ASN1TYPE_Counter:		return encodeInteger( pduVariable.toCounter(), true );
	case ASN1TYPE_Counter64:	return encodeInteger( pduVariable.toCounter64(), true );
	case ASN1TYPE_Integer64:	return encodeInteger( pduVariable.toInteger64(), false );
	case ASN1TYPE_Unsigned64:	return encodeInteger( pduVariable.toUnsigned64(), false );
	case ASN1TYPE_OBJECTID:		return encodeObjectIdentifier( pduVariable.toOID() );
	case ASN1TYPE_OCTETSTRING:	return encodeOctetString( pduVariable.toOctetString() );
	case ASN1TYPE_IPv4Address:	return encodeIPv4Address( pduVariable.toIPV4() );
	default:
		break;
	}
	Q_ASSERT(false);
	return encodeNULL();
}

bool Encoder::decodeComplex(ErrorCode &errorCode, const StdCharVector &ba, long long &pos, long long &length, ASN1::DataType &asn1Type, StdCharVectorList &baList)
{
	Q_UNUSED(baList);
	if( !getTLVData(errorCode, ba, pos, asn1Type, length) )
		return false;
	if( (asn1Type != ASN1TYPE_Sequence) && !(asn1Type & 0x80) )
	{
		errorCode = ErrorCode::WrongType;
		return false;
	}
	return true;
}

StdCharVector Encoder::encodeList(ASN1::DataType asn1Type, const StdCharVectorList &baList)
{
	StdCharVector rtn;

	long long length = 0;
	for( const StdCharVector &ba : baList )
		length += ba.count();

	rtn.append( asn1Type );
	setLength( rtn, 1, length );
	long long curPos = rtn.count();

	rtn.resize( length + rtn.count() );

	for( const StdCharVector &ba : baList )
		for( char byte : ba )
			rtn[curPos++] = byte;

	return rtn;
}

// Despite de name, it doesn't decode anything becaus doesn't knows the structure. Just puts "pos" variable on data position.
bool Encoder::decodeSequence(ErrorCode &errorCode, const StdCharVector &ba, long long &pos, long long &length)
{
	if( !getTLVData(errorCode, ba, pos, ASN1TYPE_Sequence, length, 0) )
		return false;

	return true;
}

StdCharVector Encoder::encodeSequence(const StdCharVectorList &baList)
{
	return encodeList( ASN1TYPE_Sequence, baList );
}

bool Encoder::decodePDURequest(ErrorCode &errorCode, const StdCharVector &ba, long long &pos)
{
	long long length;
	ASN1::DataType dataType;
	static std::vector<ASN1::DataType> validPDUTypes = {
		ASN1TYPE_GetRequestPDU,
		ASN1TYPE_SetRequestPDU,
		ASN1TYPE_ResponcePDU
	};

	if( !getTLVData(errorCode, ba, pos, validPDUTypes, dataType, length, 0, 0x7FFFFFFF) )
		return false;

	return true;
}

} // namespace ASN1
