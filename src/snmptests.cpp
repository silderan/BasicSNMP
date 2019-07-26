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

#include "snmptests.h"

#include "lib/asn1types.h"
#include "lib/asn1encoder.h"
#include "lib/asn1variable.h"
#include "lib/snmpencoder.h"
#include "lib/snmptable.h"

#include <iostream>

#include "qconstantsstrings.h"

using namespace SNMP;
template<typename T>
void testOneInteger(T value, const char *typeName, bool isUnsigned, const char *buff, int buffSize)
{
	T newVal;
	StdByteVector ba(buff, buffSize);
	std::cout << ((ASN1Encoder::encodeInteger(static_cast<T>(value), ASN1TYPE_INTEGER, isUnsigned) == ba) ? "Ok" : "Fail") << " setInteger<"<<typeName<<">("<<value<<") " << std::endl;

	Int64 pos = 0;
	ASN1Encoder::ErrorCode errorCode = ASN1Encoder::ErrorCode::NoError;
	ASN1Encoder::decodeInteger( errorCode, newVal, ba, pos, isUnsigned );
	std::cout << ((newVal == value) ? "Ok" : "Fail") << " getInteger<" << typeName << ">("<<value<<") " << newVal << " Error code=" << errorCode << std::endl;
}

#define TEST_ONE_INTEGER(__value, __type, __isSigned, __buff ) (testOneInteger(static_cast<__type>(__value), #__type, __isSigned, __buff, (sizeof(__buff)/sizeof(__buff[0]))-1))

void testIntegers()
{
	TEST_ONE_INTEGER( 0xFF7F, qint16, false, "\x02\x02\xFF\x7F" );
	TEST_ONE_INTEGER( 0xFFFF, quint16, true, "\x02\x03\x00\xFF\xFF" );
	TEST_ONE_INTEGER( 0xFFFFFFFF, quint32, true, "\x02\x05\x00\xFF\xFF\xFF\xFF" );
	TEST_ONE_INTEGER( 0xFFFFFFFFFFFFFFFF, quint64, true, "\x02\x09\x00\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF" );

	TEST_ONE_INTEGER( 0xFF00, qint16, false, "\x02\x02\xFF\x00" );
	TEST_ONE_INTEGER( 0xFF00, qint32, false, "\x02\x03\x00\xFF\x00" );
	TEST_ONE_INTEGER( 0xFF00, qint64, false, "\x02\x03\x00\xFF\x00" );
	TEST_ONE_INTEGER( 0xFF00, quint16, true, "\x02\x03\x00\xFF\x00" );
	TEST_ONE_INTEGER( 0xFF00, quint32, true, "\x02\x03\x00\xFF\x00" );
	TEST_ONE_INTEGER( 0xFF00, quint64, true, "\x02\x03\x00\xFF\x00" );

	TEST_ONE_INTEGER( 0xFF0000, qint32, false, "\x02\x04\x00\xFF\x00\x00" );
	TEST_ONE_INTEGER( 0xFF0000, qint64, false, "\x02\x04\x00\xFF\x00\x00" );
	TEST_ONE_INTEGER( 0xFF0000, quint32, true, "\x02\x04\x00\xFF\x00\x00" );
	TEST_ONE_INTEGER( 0xFF0000, quint64, true, "\x02\x04\x00\xFF\x00\x00" );

	TEST_ONE_INTEGER( 0xFF000000, qint32, false, "\x02\x04\xFF\x00\x00\x00" );
	TEST_ONE_INTEGER( 0xFF000000, qint64, false, "\x02\x05\x00\xFF\x00\x00\x00" );
	TEST_ONE_INTEGER( 0xFF000000, quint32, true, "\x02\x05\x00\xFF\x00\x00\x00" );
	TEST_ONE_INTEGER( 0xFF000000, quint64, true, "\x02\x05\x00\xFF\x00\x00\x00" );

	TEST_ONE_INTEGER( 0xFF00000000, qint64, false, "\x02\x06\x00\xFF\x00\x00\x00\x00" );
	TEST_ONE_INTEGER( 0xFF00000000, quint64, true, "\x02\x06\x00\xFF\x00\x00\x00\x00" );

	TEST_ONE_INTEGER( 0xFF0000000000, qint64, false, "\x02\x07\x00\xFF\x00\x00\x00\x00\x00" );
	TEST_ONE_INTEGER( 0xFF0000000000, quint64, true, "\x02\x07\x00\xFF\x00\x00\x00\x00\x00" );

	TEST_ONE_INTEGER( 0xFF000000000000, qint64, false, "\x02\x08\x00\xFF\x00\x00\x00\x00\x00\x00" );
	TEST_ONE_INTEGER( 0xFF000000000000, quint64, true, "\x02\x08\x00\xFF\x00\x00\x00\x00\x00\x00" );

	TEST_ONE_INTEGER( 0xFF00000000000000, qint64, false, "\x02\x08\xFF\x00\x00\x00\x00\x00\x00\x00" );
	TEST_ONE_INTEGER( 0xFF00000000000000, quint64, true, "\x02\x09\x00\xFF\x00\x00\x00\x00\x00\x00\x00" );

	TEST_ONE_INTEGER( 0xFF000000, int, false, "\x02\x04\xFF\x00\x00\x00" );
	TEST_ONE_INTEGER( 0xFF00, int, false, "\x02\x03\x00\xFF\x00" );
	TEST_ONE_INTEGER( 0xFF0000, int, false, "\x02\x04\x00\xFF\x00\x00" );
	TEST_ONE_INTEGER( 0xFF000000, int, false, "\x02\x04\xFF\x00\x00\x00" );
	TEST_ONE_INTEGER( -255, int, false, "\x02\x02\xFF\x01" );
	TEST_ONE_INTEGER( -256, int, false, "\x02\x02\xFF\x00" );
	TEST_ONE_INTEGER( -129, int, false, "\x02\x02\xFF\x7F" );
	TEST_ONE_INTEGER( -127, int, false, "\x02\x01\x81" );
	TEST_ONE_INTEGER( -128, int, false, "\x02\x01\x80" );
	TEST_ONE_INTEGER( 255, int, false, "\x02\x02\x00\xFF" );
	TEST_ONE_INTEGER( 0, int, false, "\x02\x01\x00" );
	TEST_ONE_INTEGER( 0x7FFFFFFF, int, false, "\x02\x04\x7F\xFF\xFF\xFF" );
	TEST_ONE_INTEGER( 0xFFFFFFFF, int, false, "\x02\x01\xFF" );

	TEST_ONE_INTEGER( 255, quint16, true, "\x02\x02\x00\xFF" );
	TEST_ONE_INTEGER( 256, quint16, true, "\x02\x02\x01\x00" );
	TEST_ONE_INTEGER( 129, quint16, true, "\x02\x02\x00\x81" );
	TEST_ONE_INTEGER( 127, quint16, true, "\x02\x01\x7F" );
	TEST_ONE_INTEGER( 128, quint16, true, "\x02\x02\x00\x80" );
	TEST_ONE_INTEGER( 0, quint16, true, "\x02\x01\x00" );
	TEST_ONE_INTEGER( 1, quint16, true, "\x02\x01\x01" );
}

void testOneOctetString(const StdString &value, const char *buff, int buffSize)
{
	StdByteVector ba(buff, buffSize);
	std::cout << ((ASN1Encoder::encodeOctetString(value) == ba) ? "Ok" : "Fail") << " setOctetString("<< value <<") " << std::endl;

	Int64 pos = 0;
	ASN1Encoder::ErrorCode errorCode;
	ASN1Variable asn1Var;
	ASN1Encoder::decodeUnknown( errorCode, ba, pos, asn1Var, nullptr );
	std::cout << (asn1Var.toStdString() == value ? "Ok" : "Fail") << " getOctetString() " << " " << asn1Var.toStdString() << " ErrorCode=" << errorCode << std::endl;

	std::cout << std::endl;
}

#define TEST_ONE_OCTET_STRING(__value, __buff ) (testOneOctetString(__value, __buff, (sizeof(__buff)/sizeof(__buff[0]))-1))

void testOctetStrings()
{
	TEST_ONE_OCTET_STRING("", "\x04\x00");
	TEST_ONE_OCTET_STRING("Hello, world", "\x04\x0CHello, world");
	TEST_ONE_OCTET_STRING("One text very long to test the enconder/decoder. Could be a Lorem Imps... whatever. But I prefer it."
						  "One text very long to test the enconder/decoder. Could be a Lorem Imps... whatever. But I prefer it."
						  "One text very long to test the enconder/decoder. Could be a Lorem Imps... whatever. But I prefer it.",
						  "\x04\x82\x01\x2C"
						  "One text very long to test the enconder/decoder. Could be a Lorem Imps... whatever. But I prefer it."
						  "One text very long to test the enconder/decoder. Could be a Lorem Imps... whatever. But I prefer it."
						  "One text very long to test the enconder/decoder. Could be a Lorem Imps... whatever. But I prefer it.");
}

void testOneOID(const OID &oid, const char *buff, int buffSize)
{
	StdByteVector ba(buff, buffSize);
	std::cout << ((ASN1Encoder::encodeObjectIdentifier(oid) == ba) ? "Ok" : "Fail") << " encodeObjectIdentifier("<<oid.toStdString()<<") " << std::endl;

	Int64 pos = 0;
	ASN1Encoder::ErrorCode errorCode;
	ASN1Variable asn1Var;
	ASN1Encoder::decodeUnknown( errorCode, ba, pos, asn1Var, nullptr );
	std::cout << ((asn1Var.toOID() == oid) ? "Ok" : "Fail") << " decodeObjectIdentifier() " << asn1Var.toOID().toStdString() << " ErrorCode=" << errorCode << std::endl;

	std::cout << std::endl;
}
#define TEST_ONE_OID(__value, __buff ) (testOneOID(__value, __buff, (sizeof(__buff)/sizeof(__buff[0]))-1))

void testOIDs()
{
	TEST_ONE_OID("1.3.2680", "\x06\x03\x2B\x94\x78");
	TEST_ONE_OID("1.3.6.1.4.1.2680.1.2.7.3.2.0", "\x06\x0D\x2B\x06\x01\x04\x01\x94\x78\x01\x02\x07\x03\x02\x00");
	TEST_ONE_OID("1.3.6.1.4.1.2.0.1.2.7.3.2.0", "\x06\x0D\x2B\x06\x01\x04\x01\x02\x00\x01\x02\x07\x03\x02\x00");
	TEST_ONE_OID("1.3", "\x06\x01\x2B");
	TEST_ONE_OID("1.3.1", "\x06\x02\x2B\x01");
}

void testOneNULL(const char *buff, int buffSize)
{
	StdByteVector ba(buff, buffSize);
	std::cout << ((ASN1Encoder::encodeNULL() == ba) ? "Ok" : "Fail") << " encodeNULL()" << std::endl;

	Int64 pos = 0;
	ASN1Encoder::ErrorCode errorCode;
	std::cout << (ASN1Encoder::decodeNULL(errorCode, ba, pos) ? "Ok" : "Fail") << " decodeNULL() " << " ErrorCode=" << errorCode << std::endl;

	std::cout << std::endl;
}
#define TEST_ONE_NULL(__buff) (testOneNULL(__buff, (sizeof(__buff)/sizeof(__buff[0]))-1))

void testNULLs()
{
	TEST_ONE_NULL("\x05\x00");
}

void testSNMPRequest()
{
	SNMP::Encoder snmpRequest;
	snmpRequest.setComunity( "private" );
	snmpRequest.setVersion( 0 );	// SNMPv1
	snmpRequest.setError( ASN1Encoder::ErrorCode::NoError, 0 );
	snmpRequest.setRequestID( 1 );
	snmpRequest.setRequestType( ASN1TYPE_GetRequestPDU );
	snmpRequest.setObjectIdentifier( "1.3.6.1.4.1.2680.1.2.7.3.2.0" );
	const char buff[] = "\x30\x2c\x02\x01\x00\x04\x07\x70\x72\x69\x76\x61\x74\x65\xA0\x1E\x02\x01\x01\x02\x01\x00\x02\x01\x00\x30\x13\x30\x11\x06\x0D\x2B\x06\x01\x04\x01\x94\x78\x01\x02\x07\x03\x02\x00\x05\x00";
	int length = sizeof(buff);
	length /= sizeof(buff[0]);
	StdByteVector checker( buff, length-1 );

	StdByteVector encoded = snmpRequest.encodeRequest();
	std::cout << ((encoded == checker) ? "Ok" : "Fail") << " SNMPDecoder::encodeRequest()" << std::endl;

	SNMP::Encoder snmpResponce;
	if( !snmpResponce.decodeAll(encoded, true) )
		std::cout << "Error in decodeAll: " << snmpResponce.errorCode() << ". Object: " << snmpResponce.errorObjectIndex() << std::endl;
	std::cout << std::endl;
}

void SNMPTests::doTests()
{
	testIntegers();
	testOctetStrings();
	testOIDs();
	testNULLs();
	testSNMPRequest();
}
