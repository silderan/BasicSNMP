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

#include "qconstantsstrings.h"
#include "utils.h"

namespace SNMPConstants {

using namespace SNMP;

const QMap<ASN1DataType, QString> &asn1TypeMap()
{
	static QMap<ASN1DataType, QString> info {
		{ ASN1TYPE_BOOLEAN,			"Primitive_Boolean" },
		{ ASN1TYPE_INTEGER,			"Primitive_Integer" },
		{ ASN1TYPE_BITSTRING,		"Primitive_BitString" },
		{ ASN1TYPE_OCTETSTRING,		"Primitive_OctetString" },
		{ ASN1TYPE_NULL,			"Primitive_Null" },
		{ ASN1TYPE_OBJECTID,		"Primitive_ObjectID" },
		{ ASN1TYPE_OBJECTDESC,		"Primitive_ObjectDescriptor (OID)" },
		{ ASN1TYPE_EXTERNAL,		"Primitive_External" },
		{ ASN1TYPE_REAL,			"Primitive_Real" },
		{ ASN1TYPE_ENUMERATED,		"Primitive_Enumerated" },

		{ ASN1TYPE_SEQUENCE,		"Primitive_Sequence" },
		{ ASN1TYPE_Sequence,		"Constructed_Sequence" },

		{ ASN1TYPE_NumericString,	"Primitive_NumericString" },
		{ ASN1TYPE_PrintableString,	"Primitive_PrintableString" },
		{ ASN1TYPE_TeletextString,	"Primitive_TeletextString" },
		{ ASN1TYPE_VideoString,		"Primitive_VideoString" },
		{ ASN1TYPE_IA5String,		"Primitive_IA5String" },

		{ ASN1TYPE_UTCTime,			"Primitive_UTCTime" },
		{ ASN1TYPE_GeneralizeTime,	"Primitive_GeneralizeTime" },
		{ ASN1TYPE_GraphicString,	"Primitive_GraphicString" },
		{ ASN1TYPE_VissibleString,	"Primitive_VissibleString" },
		{ ASN1TYPE_GeneralString,	"Primitive_GeneralString" },
		{ ASN1TYPE_CharacterString,	"Primitive_CharacterString" },

		{ ASN1TYPE_IPv4Address,		"Complex_IPv4Address" },
		{ ASN1TYPE_Counter,			"Complex_Counter" },
		{ ASN1TYPE_Gauge,			"Complex_Gauge" },
		{ ASN1TYPE_TimeTicks,		"Complex_TimeTicks" },
		{ ASN1TYPE_Opaque,			"Complex_Opaque" },
		{ ASN1TYPE_NsapAdderss,		"Complex_NsapAddress" },
		{ ASN1TYPE_Counter64,		"Complex_Counter64" },
		{ ASN1TYPE_Float,			"Complex_Float" },
		{ ASN1TYPE_Double,			"Complex_Double" },
		{ ASN1TYPE_Integer64,		"Complex_Integer64" },
		{ ASN1TYPE_Unsigned64,		"Complex_Unsigned64" },

		{ ASN1TYPE_GetRequestPDU,		"Contex_Constructed_GetRequestPDU" },
		{ ASN1TYPE_GetNextRequestPDU,	"Contex_Constructed_GetNextRequestPDU" },
		{ ASN1TYPE_ResponcePDU,			"Contex_Constructed_ResponcePDU" },
		{ ASN1TYPE_SetRequestPDU,		"Contex_Constructed_SetRequestPDU" },
		{ ASN1TYPE_TrapPDU,				"Contex_Constructed_TrapPDU" }
		};

	return info;
}

QString asn1TypeName(ASN1DataType type)
{
	return asn1TypeMap().value(type, QString("UnknownASN1Type_%1").arg(type, 2, 16, QChar('0')));
}

QString asn1PrintableValue(const ASN1Variable &asn1Var)
{
	switch( asn1Var.type() )
	{
	case ASN1TYPE_BOOLEAN:		return asn1Var.toBoolean() ? "true" : "false";
	case ASN1TYPE_INTEGER:		return QString::number( asn1Var.toUInteger() );
	case ASN1TYPE_NULL:			return "<null>";
	case ASN1TYPE_OBJECTID:		return QString::fromStdString( asn1Var.toOID().toStdString() );

	case ASN1TYPE_OCTETSTRING:
	case ASN1TYPE_NumericString:
	case ASN1TYPE_TeletextString:
	case ASN1TYPE_VideoString:
	case ASN1TYPE_IA5String:
	case ASN1TYPE_GraphicString:
	case ASN1TYPE_GeneralString:
	case ASN1TYPE_CharacterString:
		return QString::fromStdString( asn1Var.toStdString() );

	case ASN1TYPE_UTCTime:			return QString("%1 secs").arg(asn1Var.toUnsigned64()/100 );
	case ASN1TYPE_GeneralizeTime:	return QString("%1 secs").arg(asn1Var.toUnsigned64()/100 );

	case ASN1TYPE_Counter:		return QString::number( asn1Var.toCounter() );
	case ASN1TYPE_Gauge:		return QString::number( asn1Var.toGauge() );
	case ASN1TYPE_TimeTicks:	return QString("%1 secs").arg(asn1Var.toUnsigned64()/100 );
//	case ASN1TYPE_Float:		return QString::number( asn1Var.toFloat() );
//	case ASN1TYPE_Double:		return QString::number( asn1Var.toDouble() );
	case ASN1TYPE_Counter64:	return QString::number( asn1Var.toCounter64() );
	case ASN1TYPE_Integer64:	return QString::number( asn1Var.toInteger64() );
	case ASN1TYPE_Unsigned64:	return QString::number( asn1Var.toUnsigned64() );

	case ASN1TYPE_IPv4Address:	return QString::fromStdString( Utils::ipv4AddressToStdString(asn1Var.toIPV4()) );
	}
	return QString("<no displayable>");
}

QString printableErrorCode(ASN1Encoder::ErrorCode ec)
{
	switch( ec )
	{
	case ASN1Encoder::ErrorCode::DatagramInterrupted:	return "DatagramInterrupted";
	case ASN1Encoder::ErrorCode::UnsignedMalformed:	return "UnsignedMalformed";
	case ASN1Encoder::ErrorCode::NotEnoughRoom:		return "NotEnoughRoom";
	case ASN1Encoder::ErrorCode::NoError:				return "NoError";
	case ASN1Encoder::ErrorCode::TooBig:				return "TooBig";
	case ASN1Encoder::ErrorCode::NoSuchName:			return "NoSuchName";
	case ASN1Encoder::ErrorCode::ReadOnly:			return "ReadOnly";
	case ASN1Encoder::ErrorCode::BadValue:			return "Bad Value";
	case ASN1Encoder::ErrorCode::GenericError:		return "Generic error";
	case ASN1Encoder::ErrorCode::NoAccess:			return "No accesible";
	case ASN1Encoder::ErrorCode::WrongType:			return "Wrong type";
	case ASN1Encoder::ErrorCode::WrongLength:			return "Wrong length";
	case ASN1Encoder::ErrorCode::WrongEncoding:		return "Wrong encoding";
	case ASN1Encoder::ErrorCode::WrongValue:			return "Wong value";
	case ASN1Encoder::ErrorCode::NoCreation:			return "Cannot create";
	case ASN1Encoder::ErrorCode::InconsistentValue:	return "Value inconsistent with other object values";
	case ASN1Encoder::ErrorCode::ResourceUnavailable:	return "No resources available";
	case ASN1Encoder::ErrorCode::CommitFailed:		return "Commit failed; no variables updated";
	case ASN1Encoder::ErrorCode::UndoFailed:			return "Some variables were updated because undo was not possible";
	case ASN1Encoder::ErrorCode::AuthorizationError:	return "Authorization error";
	case ASN1Encoder::ErrorCode::NotWritable:			return "Canot modify object";
	case ASN1Encoder::ErrorCode::InconsistentName:	return "InconsistentName: Cannot create new object because its inconsistent with other objects";
	}
	return QString("Encoder::ErrorCode_Unknown_0x%1").arg(ec, 2, 16, QChar('0') );
}

QString printableErrorCode(const Encoder &snmp)
{
	// TODO: Mirar de añadir algún tipo de campo más adicional donde guardar ciertos
	// datos del error que dependan del contexto. Por ejemplo, si el error es del tamaño,
	// que se pueda obtener el tamaño mínimo y el máximo.
	if( snmp.errorObjectIndex() != 0 )
		return QString("%1 in object %2").arg(printableErrorCode(snmp.errorCode())).arg(snmp.errorObjectIndex());

	return printableErrorCode(snmp.errorCode());
}

const QMap<EntryStatus, QString> &entryStatusInfoMap()
{
	static QMap<EntryStatus, QString> infoMap =
	{
		{EntryStatus::valid,			"EntryStatus: valid"},
		{EntryStatus::createRequest,	"EntryStatus: createRequest"},
		{EntryStatus::underCreation,	"EntryStatus: underCreation"},
		{EntryStatus::invalid,		"EntryStatus: invalid"}
	};
	return infoMap;
}

QString entryStatusName(EntryStatus es)
{
	return entryStatusInfoMap().value( es, "EntryStatus: error-code" );
}

const QMap<StatusRow, QString> &statusRowInfoMap()
{
	static QMap<StatusRow, QString> infoMap =
	{
		{StatusRow::active,			"EntryStatus: valid"},
		{StatusRow::notInService,		"EntryStatus: notInService"},
		{StatusRow::notReady,			"EntryStatus: notReady"},
		{StatusRow::createAndGo,		"EntryStatus: createAndGo"},
		{StatusRow::createAndWait,	"EntryStatus: createAndWait"},
		{StatusRow::destroy,			"EntryStatus: destroy"}
	};
	return infoMap;
}

QString statusRowName(StatusRow sr)
{
	return statusRowInfoMap().value( sr, "EntryStatus: error-code" );
}

};
