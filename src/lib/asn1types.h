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

#ifndef ASN1TYPES_H
#define ASN1TYPES_H

#include "basic_types.h"
namespace SNMP
{

typedef Byte ASN1DataType;
}

#define ASN1TYPECLASS_UNIVERSAL			(static_cast<SNMP::ASN1DataType>(0x00))
#define ASN1TYPECLASS_APPLICATION		(static_cast<SNMP::ASN1DataType>(0x40))
#define ASN1TYPECLASS_CONTEXT_SPECIFIC	(static_cast<SNMP::ASN1DataType>(0x80))
#define ASN1TYPECLASS_PRIVATE			(static_cast<SNMP::ASN1DataType>(0xC0))

#define ASN1TYPEBASE_PRIMITIVE			(static_cast<SNMP::ASN1DataType>(0x00))
#define ASN1TYPEBASE_CONSTRUCTED		(static_cast<SNMP::ASN1DataType>(0x20))

#define ASN1TYPE_BOOLEAN				(ASN1TYPECLASS_UNIVERSAL | ASN1TYPEBASE_PRIMITIVE | static_cast<SNMP::ASN1DataType>(0x01))	// 0x01
#define ASN1TYPE_INTEGER				(ASN1TYPECLASS_UNIVERSAL | ASN1TYPEBASE_PRIMITIVE | static_cast<SNMP::ASN1DataType>(0x02))	// 0x02
#define ASN1TYPE_BITSTRING				(ASN1TYPECLASS_UNIVERSAL | ASN1TYPEBASE_PRIMITIVE | static_cast<SNMP::ASN1DataType>(0x03))	// 0x03
#define ASN1TYPE_OCTETSTRING			(ASN1TYPECLASS_UNIVERSAL | ASN1TYPEBASE_PRIMITIVE | static_cast<SNMP::ASN1DataType>(0x04))	// 0x04
#define ASN1TYPE_NULL					(ASN1TYPECLASS_UNIVERSAL | ASN1TYPEBASE_PRIMITIVE | static_cast<SNMP::ASN1DataType>(0x05))	// 0x05
#define ASN1TYPE_OBJECTID				(ASN1TYPECLASS_UNIVERSAL | ASN1TYPEBASE_PRIMITIVE | static_cast<SNMP::ASN1DataType>(0x06))	// 0x06
#define ASN1TYPE_OBJECTDESC				(ASN1TYPECLASS_UNIVERSAL | ASN1TYPEBASE_PRIMITIVE | static_cast<SNMP::ASN1DataType>(0x07))	// 0x07
#define ASN1TYPE_EXTERNAL				(ASN1TYPECLASS_UNIVERSAL | ASN1TYPEBASE_PRIMITIVE | static_cast<SNMP::ASN1DataType>(0x08))	// 0x08
#define ASN1TYPE_REAL					(ASN1TYPECLASS_UNIVERSAL | ASN1TYPEBASE_PRIMITIVE | static_cast<SNMP::ASN1DataType>(0x09))	// 0x09
#define ASN1TYPE_ENUMERATED				(ASN1TYPECLASS_UNIVERSAL | ASN1TYPEBASE_PRIMITIVE | static_cast<SNMP::ASN1DataType>(0x0A))	// 0x0A

#define ASN1TYPE_SEQUENCE				(ASN1TYPECLASS_UNIVERSAL | ASN1TYPEBASE_PRIMITIVE | static_cast<SNMP::ASN1DataType>(0x10))	// 0x10
#define ASN1TYPE_Sequence				(ASN1TYPECLASS_UNIVERSAL | ASN1TYPEBASE_CONSTRUCTED | static_cast<SNMP::ASN1DataType>(0x10))// 0x30

#define ASN1TYPE_NumericString			(ASN1TYPECLASS_UNIVERSAL | ASN1TYPEBASE_PRIMITIVE | static_cast<SNMP::ASN1DataType>(0x12))	// 0x12
#define ASN1TYPE_PrintableString		(ASN1TYPECLASS_UNIVERSAL | ASN1TYPEBASE_PRIMITIVE | static_cast<SNMP::ASN1DataType>(0x13))	// 0x13
#define ASN1TYPE_TeletextString			(ASN1TYPECLASS_UNIVERSAL | ASN1TYPEBASE_PRIMITIVE | static_cast<SNMP::ASN1DataType>(0x14))	// 0x14
#define ASN1TYPE_VideoString			(ASN1TYPECLASS_UNIVERSAL | ASN1TYPEBASE_PRIMITIVE | static_cast<SNMP::ASN1DataType>(0x15))	// 0x15
#define ASN1TYPE_IA5String				(ASN1TYPECLASS_UNIVERSAL | ASN1TYPEBASE_PRIMITIVE | static_cast<SNMP::ASN1DataType>(0x16))	// 0x16

#define ASN1TYPE_UTCTime				(ASN1TYPECLASS_UNIVERSAL | ASN1TYPEBASE_PRIMITIVE | static_cast<SNMP::ASN1DataType>(0x17))	// 0x17
#define ASN1TYPE_GeneralizeTime			(ASN1TYPECLASS_UNIVERSAL | ASN1TYPEBASE_PRIMITIVE | static_cast<SNMP::ASN1DataType>(0x18))	// 0x18
#define ASN1TYPE_GraphicString			(ASN1TYPECLASS_UNIVERSAL | ASN1TYPEBASE_PRIMITIVE | static_cast<SNMP::ASN1DataType>(0x19))	// 0x19
#define ASN1TYPE_VissibleString			(ASN1TYPECLASS_UNIVERSAL | ASN1TYPEBASE_PRIMITIVE | static_cast<SNMP::ASN1DataType>(0x1A))	// 0x1A
#define ASN1TYPE_GeneralString			(ASN1TYPECLASS_UNIVERSAL | ASN1TYPEBASE_PRIMITIVE | static_cast<SNMP::ASN1DataType>(0x1B))	// 0x1B
#define ASN1TYPE_CharacterString		(ASN1TYPECLASS_UNIVERSAL | ASN1TYPEBASE_PRIMITIVE | static_cast<SNMP::ASN1DataType>(0x1C))	// 0x1C

#define ASN1TYPE_IPv4Address			(ASN1TYPECLASS_APPLICATION | ASN1TYPEBASE_PRIMITIVE | static_cast<SNMP::ASN1DataType>(0x00))	// 0x40
#define ASN1TYPE_Counter				(ASN1TYPECLASS_APPLICATION | ASN1TYPEBASE_PRIMITIVE | static_cast<SNMP::ASN1DataType>(0x01))	// 0x41
#define ASN1TYPE_Gauge32				(ASN1TYPECLASS_APPLICATION | ASN1TYPEBASE_PRIMITIVE | static_cast<SNMP::ASN1DataType>(0x02))	// 0x42
#define ASN1TYPE_TimeTicks				(ASN1TYPECLASS_APPLICATION | ASN1TYPEBASE_PRIMITIVE | static_cast<SNMP::ASN1DataType>(0x03))	// 0x43
#define ASN1TYPE_Opaque					(ASN1TYPECLASS_APPLICATION | ASN1TYPEBASE_PRIMITIVE | static_cast<SNMP::ASN1DataType>(0x04))	// 0x44
#define ASN1TYPE_NsapAdderss			(ASN1TYPECLASS_APPLICATION | ASN1TYPEBASE_PRIMITIVE | static_cast<SNMP::ASN1DataType>(0x05))	// 0x45
#define ASN1TYPE_Counter64				(ASN1TYPECLASS_APPLICATION | ASN1TYPEBASE_PRIMITIVE | static_cast<SNMP::ASN1DataType>(0x06))	// 0x46
#define ASN1TYPE_Float					(ASN1TYPECLASS_APPLICATION | ASN1TYPEBASE_PRIMITIVE | static_cast<SNMP::ASN1DataType>(0x08))	// 0x48
#define ASN1TYPE_Double					(ASN1TYPECLASS_APPLICATION | ASN1TYPEBASE_PRIMITIVE | static_cast<SNMP::ASN1DataType>(0x09))	// 0x49
#define ASN1TYPE_Integer64				(ASN1TYPECLASS_APPLICATION | ASN1TYPEBASE_PRIMITIVE | static_cast<SNMP::ASN1DataType>(0x10))	// 0x50
#define ASN1TYPE_Unsigned64				(ASN1TYPECLASS_APPLICATION | ASN1TYPEBASE_PRIMITIVE | static_cast<SNMP::ASN1DataType>(0x11))	// 0x51

#define ASN1TYPE_GetRequestPDU			(ASN1TYPECLASS_CONTEXT_SPECIFIC | ASN1TYPEBASE_CONSTRUCTED | static_cast<SNMP::ASN1DataType>(0x00))	// 0xA0
#define ASN1TYPE_GetNextRequestPDU		(ASN1TYPECLASS_CONTEXT_SPECIFIC | ASN1TYPEBASE_CONSTRUCTED | static_cast<SNMP::ASN1DataType>(0x01))	// 0xA1
#define ASN1TYPE_ResponcePDU			(ASN1TYPECLASS_CONTEXT_SPECIFIC | ASN1TYPEBASE_CONSTRUCTED | static_cast<SNMP::ASN1DataType>(0x02))	// 0xA2
#define ASN1TYPE_SetRequestPDU			(ASN1TYPECLASS_CONTEXT_SPECIFIC | ASN1TYPEBASE_CONSTRUCTED | static_cast<SNMP::ASN1DataType>(0x03))	// 0xA3
#define ASN1TYPE_TrapPDU				(ASN1TYPECLASS_CONTEXT_SPECIFIC | ASN1TYPEBASE_CONSTRUCTED | static_cast<SNMP::ASN1DataType>(0x04))	// 0xA4


#endif // ASN1TYPES_H
