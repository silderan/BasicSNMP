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

using namespace SNMP;

bool ASN1Encoder::checkDataType(ErrorCode &errorCode, ASN1DataType actualType, ASN1DataType validType)
{
	if( actualType != validType )
	{
		errorCode = ErrorCode::WrongType;
		return false;
	}
	return true;
}

bool ASN1Encoder::checkDataType(ErrorCode &errorCode, ASN1DataType actualType, const StdVector<ASN1DataType> &validTypes)
{
	if( validTypes.size() && !Utils::contains(validTypes, actualType) )
	{
		errorCode = ErrorCode::WrongType;
		return false;
	}
	return true;
}

bool ASN1Encoder::checkDataLength(ErrorCode &errorCode, const StdByteVector &ba, Int64 dataPos, Int64 actualLength, Int64 minLength, Int64 maxLength)
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
	if( (dataPos + actualLength) > static_cast<UInt32>(ba.count()) )
	{
		errorCode = ErrorCode::NotEnoughRoom;
	}
	else
		return true;
	return false;
}

bool ASN1Encoder::checkDataRoom(ErrorCode &errorCode, const StdByteVector &ba, Int64 initialDataPos, Int64 dataLength)
{
	if( (ba.count() - initialDataPos) < dataLength )
	{
		errorCode = ErrorCode::NotEnoughRoom;
		return false;
	}
	return true;
}

bool ASN1Encoder::getTLVData(ErrorCode &errorCode,
							const StdByteVector &ba,	// Byte Array received from SNMP agent.
							Int64 &initialPos,			// Initial pos in the array to analize. Will point to data position if no error.
							const std::vector<ASN1DataType> &validTypeList,	// Valid data type.
							ASN1DataType &actualType,	// Actual data received. Will be set if no error.
							Int64 &length,				// The length of the data to retrieve. Will be set if no error.
							Int64 minDataLength,		// The minimim data required. usually, it's 1.
							Int64 maxDataLength)		// Max data length for data. For example. if a signed 32 integer is spected, data length should be 4. 5 if it's a UInt32eger because of the 00 leading byte needed for this.
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

bool ASN1Encoder::getTLVData(ErrorCode &errorCode,
							const StdByteVector &ba,
							Int64 &initialPos,
							ASN1DataType validType,
							Int64 &length,
							Int64 minDataLength,
							Int64 maxDataLength)
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

bool ASN1Encoder::getTLVData(ErrorCode &errorCode,
							const StdByteVector &ba,
							Int64 &initialPos,
							ASN1DataType &validType,
							Int64 &length)
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

bool ASN1Encoder::getLength(ErrorCode &errorCode, const StdByteVector &ba, Int64 &pos, Int64 &length)
{
	if( pos < static_cast<UInt32>(ba.size()) )
	{
		if( !(ba[pos] & 0x80) ) // Less that 127.
		{
			length = static_cast<UInt32>(ba[pos++]);
			return true;
		}
		UInt32 count = ba[pos++] & 0x7F;
		// Check if there is enough rooom in the data for all bytes.
		if( (static_cast<UInt32>(ba.size()) - pos) >= count )
		{
			length = 0;
			while( count > 0 )
			{
				length <<= 8;
				length += (0xFF & static_cast<UInt32>(ba[pos++]));
				--count;
			}
			return true;
		}
	}
	errorCode = ErrorCode::DatagramInterrupted;
	return false;
}

void ASN1Encoder::setLength(StdByteVector &ba, Int64 pos, Int64 length)
{
	// Length has two forms to be encoded.
	// 1: If value is less than 127, it's encoded directly.
	// 2: If value is greather, first byte encoded in the 7 less significant bits de quantity of bytes used to encode the actual length
	if( length < 127 )
		ba.insert( pos, static_cast<Byte>(length) );
	else
	{
		int count = static_cast<int>( ((length - 1) >> 8) + 1 );
		ba.insert( pos++, static_cast<Byte>(count | 0x80) );

		while( length != 0 )
		{
			ba.insert( pos, static_cast<Byte>(length & 0xFF) );
			length >>= 8;
		}
	}
}

bool ASN1Encoder::decodeNULL(ErrorCode &/*errorCode*/, const StdByteVector &/*ba*/, Int64 &/*pos*/, Int64 /*length*/)
{
	return true;
}

bool ASN1Encoder::decodeInteger(ErrorCode &errorCode,
							   ASN1Variable &asnVar,
							   const StdByteVector &ba,
							   Int64 &pos,
							   Int64 length)
{
	if( !(ba[pos] & 0x80) )
	{
		UInt64 ui;
		bool rtn = decodeInteger(errorCode, ui, ba, pos, length, true);
		switch( asnVar.type() )
		{
		case ASN1TYPE_INTEGER:		asnVar.setUInteger(ui);		break;
		case ASN1TYPE_Counter:		asnVar.setCounter(ui);		break;
		case ASN1TYPE_Counter64:	asnVar.setCounter64(ui);	break;
		case ASN1TYPE_Integer64:	asnVar.setUnsigned64(ui);	break;
		case ASN1TYPE_Gauge32:		asnVar.setGauge32(static_cast<UInt32>(ui));		break;
		case ASN1TYPE_TimeTicks:	asnVar.setTimeTicks(ui);	break;
		case ASN1TYPE_Unsigned64:	asnVar.setUnsigned64(ui);	break;
		}
		return rtn;
	}
	else
	{
		Int64 i;
		bool rtn = decodeInteger(errorCode, i, ba, pos, length, false);
		asnVar.setInteger(i);
		return rtn;
	}
}




StdByteVector ASN1Encoder::encodeOctetString(const StdByteVector &value)
{
	StdByteVector ba;
	ba.append( ASN1TYPE_OCTETSTRING );
	setLength( ba, 1, value.count() );
	ba.append( value );
	return ba;
}

StdByteVector ASN1Encoder::encodeOctetString(const StdString &value)
{
	StdByteVector ba;
	ba.append( ASN1TYPE_OCTETSTRING );
	setLength( ba, 1, static_cast<Int64>(value.size()) );
	ba.append( value.data() );
	return ba;
}

void ASN1Encoder::encodeObjectIdentifierValue(const OIDValue &oidValue, StdByteVector &ba)
{
	int bitShift = 63;
	UInt64 mask = 0x7F00000000000000u;
	UInt64 value = oidValue.toULongLong();
	while( mask )
	{
		if( value > mask )
			ba.append( static_cast<char>((value >> bitShift) & 0x7Fu) | 0x80 );
		mask >>= 7;
		bitShift -= 7;
	}
	ba.append( static_cast<char>(value & 0x7Fu) );
}
bool ASN1Encoder::decodeObjectIdentifierValue(ErrorCode &errorCode, const StdByteVector &ba, Int64 &pos, OIDValue &oidValue)
{
	UInt64 value = 0;
	do
	{
		if( static_cast<UInt32>(ba.count()) <= pos )
		{
			errorCode = ErrorCode::NotEnoughRoom;
			return false;
		}
		value <<= 7;
		value += ba[pos] & 0x7F;
	}
	while( ba[pos++] & 0x80 );
	oidValue = OIDValue(value);
	return true;
}

bool ASN1Encoder::decodeObjectIdentifier(ErrorCode &errorCode,
										OID &oid,
										const StdByteVector &ba,
										Int64 &pos,
										Int64 length)
{
	OIDValue value;

	oid.clear( );
	// At this point, we don't know the OID lenght, but we can make a guess.
	oid.reserve( static_cast<OID::size_type>(length + 1) );
	length += pos;

	oid.append( OIDValue(ba[pos] / 40) );
	oid.append( OIDValue(ba[pos++] % 40) );

	for( Int64 i = 2; (length > pos) && decodeObjectIdentifierValue(errorCode, ba, pos, value); ++i )
		oid.append( value );

	return true;
}
StdByteVector ASN1Encoder::encodeObjectIdentifier(const OID &oid)
{
	StdByteVector ba;
	assert( oid.count() > 1 );
	ba.append( ASN1TYPE_OBJECTID );

	// First 2 elements have special coding: 40x[0]+[1] in the first byte.
	// That is because both are allways small. 1.3.
	// Bad design, anyway: the x40 math for the first value is a nonsense, IMHO.
	ba.append( static_cast<Byte>( oid[0].toULongLong() * 40 + oid[1].toULongLong()) );

	// From 3dr element and so on, all are identical.
	for( Int64 pos = 2; pos < oid.count(); ++pos )
		encodeObjectIdentifierValue( oid[pos], ba );

	setLength( ba, 1, static_cast<UInt32>(ba.count()-1) );
	return ba;
}

bool ASN1Encoder::decodeObjectIdentifier(ErrorCode &errorCode, OID &oid, const StdByteVector &ba, Int64 &pos)
{
	Int64 length;
	if( !getTLVData(errorCode, ba, pos, ASN1TYPE_OBJECTID, length, 1) )
		return false;

	return decodeObjectIdentifier(errorCode, oid, ba, pos, length);
}

bool ASN1Encoder::decodeObjectIdentifier(ErrorCode &errorCode, ASN1Variable &asn1Var, const StdByteVector &ba, Int64 &pos)
{
	OID oid;
	if( decodeObjectIdentifier(errorCode, oid, ba, pos) )
	{
		asn1Var.setOID(oid);
		return true;
	}
	return false;
}

bool ASN1Encoder::decodeGenericString(ErrorCode &/*errorCode*/,
									 StdByteVector &value,
									 const StdByteVector &ba,
									 Int64 &pos,
									 Int64 length)
{
	value = ba.mid( pos, length );
	pos += length;
	return true;
}

bool ASN1Encoder::decodeGenericString(ErrorCode &errorCode,
									 StdByteVector &value,
									 const StdByteVector &ba,
									 Int64 &pos)
{
	ASN1DataType dataType;
	Int64 length;

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

bool ASN1Encoder::decodeOctetString(ErrorCode &errorCode, StdByteVector &value, const StdByteVector &ba, Int64 &pos)
{
	return decodeGenericString(errorCode, value, ba, pos);
}

bool ASN1Encoder::decodeNULL(ErrorCode &errorCode, const StdByteVector &ba, Int64 &pos)
{
	Int64 length;

	if( !getTLVData(errorCode, ba, pos, ASN1TYPE_NULL, length, 0, 0) )
		return false;

	return decodeNULL(errorCode, ba, pos, length);
}

StdByteVector ASN1Encoder::encodeNULL()
{
	StdByteVector ba;
	ba.append( ASN1TYPE_NULL );
	ba.append( static_cast<char>(0) );
	return ba;
}

bool ASN1Encoder::decodeIPv4Address(ErrorCode &errorCode, Utils::IPv4Address &ipv4, const StdByteVector &ba, Int64 &pos)
{
	Int64 length;
	if( !getTLVData(errorCode, ba, pos, ASN1TYPE_IPv4Address, length, 4, 4) )
		return false;

	ipv4 =	static_cast<unsigned>(static_cast<Byte>(ba[pos+0]) << 24) +
			static_cast<unsigned>(static_cast<Byte>(ba[pos+1]) << 16) +
			static_cast<unsigned>(static_cast<Byte>(ba[pos+2]) << 8) +
			static_cast<unsigned>(static_cast<Byte>(ba[pos+3])) ;

	pos+=4;
	return true;
}

bool ASN1Encoder::decodeIPv4Address(ErrorCode &errorCode, ASN1Variable &asn1Var, const StdByteVector &ba, Int64 &pos)
{
	Utils::IPv4Address ipv4;
	if( decodeIPv4Address(errorCode, ipv4, ba, pos) )
	{
		asn1Var.setIPv4(ipv4);
		return true;
	}
	return false;
}

bool ASN1Encoder::decodeIPv4Address(ErrorCode &errorCode, Utils::IPv4Address &ipv4, const StdByteVector &ba, Int64 &pos, Int64 length)
{
	if( length != 4 )
	{
		errorCode = ErrorCode::WrongLength;
		return false;
	}
	ipv4 =	static_cast<unsigned>(static_cast<Byte>(ba[pos+0]) << 24) +
			static_cast<unsigned>(static_cast<Byte>(ba[pos+1]) << 16) +
			static_cast<unsigned>(static_cast<Byte>(ba[pos+2]) << 8) +
			static_cast<unsigned>(static_cast<Byte>(ba[pos+3])) ;

	pos+=4;
	return true;
}

StdByteVector ASN1Encoder::encodeIPv4Address(const StdString &ipv4Address)
{
	StdByteVector ba;
	ba.append( ASN1TYPE_IPv4Address );
	setLength( ba, 1, 4 );

	for( const StdString &num : Utils::split(ipv4Address, '.') )
		ba.append( static_cast<Byte>(0xFF & std::stoul(num)) );

	return ba;
}

StdByteVector ASN1Encoder::encodeIPv4Address(const Utils::IPv4Address &ipv4Address)
{
	StdByteVector ba;
	ba.append( ASN1TYPE_IPv4Address );
	setLength( ba, 1, 4 );

	ba.append( ipv4Address.octetA() );
	ba.append( ipv4Address.octetB() );
	ba.append( ipv4Address.octetC() );
	ba.append( ipv4Address.octetD() );
	return ba;
}

bool ASN1Encoder::decodeUnknown(ErrorCode &errorCode,
								const StdByteVector &ba,
								Int64 &pos,
								Int64 length,
								ASN1DataType asn1Type,
								ASN1Variable &data )
{
	bool rtn;
	switch( asn1Type )
	{
	case ASN1TYPE_NULL:
		rtn = decodeNULL(errorCode, ba, pos, length);
		data.setNull();
		break;
	case ASN1TYPE_INTEGER:
	case ASN1TYPE_Gauge32:
	case ASN1TYPE_Counter:
	case ASN1TYPE_Counter64:
	case ASN1TYPE_Integer64:
	case ASN1TYPE_Unsigned64:
		data.setType(asn1Type);
		rtn = decodeInteger(errorCode, data, ba, pos, length);
		data.setType(asn1Type);
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
		StdByteVector s;
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

bool ASN1Encoder::decodeUnknown(ErrorCode &errorCode, const StdByteVector &ba, Int64 &pos, ASN1Variable &asn1Var, StdByteVector *rawData)
{
	Int64 length;
	ASN1DataType asn1Type;
	if( !getTLVData(errorCode, ba, pos, asn1Type, length) )
		return false;

	if( rawData != nullptr )
		*rawData = ba.mid( pos, length );

	return decodeUnknown(errorCode, ba, pos, length, asn1Type, asn1Var);
}

StdByteVector ASN1Encoder::encodeUnknown( const ASN1Variable &pduVariable )
{
	switch( pduVariable.type() )
	{
	case ASN1TYPE_NULL:			return encodeNULL( );
	case ASN1TYPE_INTEGER:		return encodeInteger( pduVariable.toInteger(),		pduVariable.type(), false );
	case ASN1TYPE_Gauge32:		return encodeInteger( pduVariable.toGauge32(),		pduVariable.type(), true );
	case ASN1TYPE_Counter:		return encodeInteger( pduVariable.toCounter(),		pduVariable.type(), true );
	case ASN1TYPE_Counter64:	return encodeInteger( pduVariable.toCounter64(),	pduVariable.type(), true );
	case ASN1TYPE_Integer64:	return encodeInteger( pduVariable.toInteger64(),	pduVariable.type(), false );
	case ASN1TYPE_Unsigned64:	return encodeInteger( pduVariable.toUnsigned64(),	pduVariable.type(), false );
	case ASN1TYPE_OBJECTID:		return encodeObjectIdentifier( pduVariable.toOID() );
	case ASN1TYPE_OCTETSTRING:	return encodeOctetString( pduVariable.toOctetString() );
	case ASN1TYPE_IPv4Address:	return encodeIPv4Address( pduVariable.toIPV4() );
	default:
		break;
	}
	assert(false);
	return encodeNULL();
}

bool ASN1Encoder::decodeComplex(ErrorCode &errorCode, const StdByteVector &ba, Int64 &pos, Int64 &length, ASN1DataType &asn1Type, StdByteVectorList &/*baList*/)
{
	if( !getTLVData(errorCode, ba, pos, asn1Type, length) )
		return false;
	if( (asn1Type != ASN1TYPE_Sequence) && !(asn1Type & 0x80) )
	{
		errorCode = ErrorCode::WrongType;
		return false;
	}
	return true;
}

StdByteVector ASN1Encoder::encodeList(ASN1DataType asn1Type, const StdByteVectorList &baList)
{
	StdByteVector rtn;

	Int64 length = 0;
	for( const StdByteVector &ba : baList )
		length += ba.count();

	rtn.append( asn1Type );
	setLength( rtn, 1, length );
	Int64 curPos = rtn.count();

	rtn.resize( length + rtn.count() );

	for( const StdByteVector &ba : baList )
		for( Byte byte : ba )
			rtn[curPos++] = byte;

	return rtn;
}

// Despite de name, it doesn't decode anything becaus doesn't knows the structure. Just puts "pos" variable on data position.
bool ASN1Encoder::decodeSequence(ErrorCode &errorCode, const StdByteVector &ba, Int64 &pos, Int64 &length)
{
	if( !getTLVData(errorCode, ba, pos, ASN1TYPE_Sequence, length, 0) )
		return false;

	return true;
}

StdByteVector ASN1Encoder::encodeSequence(const StdByteVectorList &baList)
{
	return encodeList( ASN1TYPE_Sequence, baList );
}

bool ASN1Encoder::decodePDURequest(ErrorCode &errorCode, const StdByteVector &ba, Int64 &pos)
{
	Int64 length;
	ASN1DataType dataType;
	static StdVector<ASN1DataType> validPDUTypes = {
		ASN1TYPE_GetRequestPDU,
		ASN1TYPE_SetRequestPDU,
		ASN1TYPE_ResponcePDU
	};

	if( !getTLVData(errorCode, ba, pos, validPDUTypes, dataType, length, 0, 0x7FFFFFFF) )
		return false;

	return true;
}

