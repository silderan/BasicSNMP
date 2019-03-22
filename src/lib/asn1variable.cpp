#include "asn1variable.h"
namespace ASN1 {

ASN1::Variable &ASN1::Variable::operator =(const ASN1::Variable &pduVar)
{
	switch( mDataType )
	{
	case ASN1TYPE_BOOLEAN:			mDataValue.number.boolean = pduVar.mDataValue.number.boolean;			break;
	case ASN1TYPE_INTEGER:			mDataValue.number.Integer64 = pduVar.mDataValue.number.Integer64;		break;
	case ASN1TYPE_BITSTRING:		mDataValue.octetString = pduVar.mDataValue.octetString;					break;
	case ASN1TYPE_OCTETSTRING:		mDataValue.octetString = pduVar.mDataValue.octetString;					break;
	case ASN1TYPE_NULL:				mDataValue.number.Integer64 = pduVar.mDataValue.number.Integer64;		break;
	case ASN1TYPE_OBJECTID:			mDataValue.oid = pduVar.mDataValue.oid;									break;
	case ASN1TYPE_OBJECTDESC:		mDataValue.octetString = pduVar.mDataValue.octetString;					break;
	case ASN1TYPE_REAL:				mDataValue.number.Float = pduVar.mDataValue.number.Float;				break;
	case ASN1TYPE_NumericString:	mDataValue.octetString = pduVar.mDataValue.octetString;					break;
	case ASN1TYPE_PrintableString:	mDataValue.octetString = pduVar.mDataValue.octetString;					break;
	case ASN1TYPE_TeletextString:	mDataValue.octetString = pduVar.mDataValue.octetString;					break;
	case ASN1TYPE_VideoString:		mDataValue.octetString = pduVar.mDataValue.octetString;					break;
	case ASN1TYPE_IA5String:		mDataValue.octetString = pduVar.mDataValue.octetString;					break;
	case ASN1TYPE_UTCTime:			mDataValue.octetString = pduVar.mDataValue.octetString;					break;
	case ASN1TYPE_GeneralizeTime:	mDataValue.octetString = pduVar.mDataValue.octetString;					break;
	case ASN1TYPE_GraphicString:	mDataValue.octetString = pduVar.mDataValue.octetString;					break;
	case ASN1TYPE_VissibleString:	mDataValue.octetString = pduVar.mDataValue.octetString;					break;
	case ASN1TYPE_GeneralString:	mDataValue.octetString = pduVar.mDataValue.octetString;					break;
	case ASN1TYPE_CharacterString:	mDataValue.octetString = pduVar.mDataValue.octetString;					break;
	case ASN1TYPE_IPv4Address:		mDataValue.octetString = pduVar.mDataValue.octetString;					break;
	case ASN1TYPE_Counter:			mDataValue.number.UInteger64 = pduVar.mDataValue.number.UInteger64;		break;
	case ASN1TYPE_Gauge:			mDataValue.number.UInteger64 = pduVar.mDataValue.number.UInteger64;		break;
	case ASN1TYPE_TimeTicks:		mDataValue.number.UInteger64 = pduVar.mDataValue.number.UInteger64;		break;
	case ASN1TYPE_Counter64:		mDataValue.number.UInteger64 = pduVar.mDataValue.number.UInteger64;		break;
	case ASN1TYPE_Float:			mDataValue.number.Float = pduVar.mDataValue.number.Float;				break;
	case ASN1TYPE_Double:			mDataValue.number.Double = pduVar.mDataValue.number.Double;				break;
	case ASN1TYPE_Integer64:		mDataValue.number.Integer64 = pduVar.mDataValue.number.Integer64;		break;
	case ASN1TYPE_Unsigned64:		mDataValue.number.UInteger64 = pduVar.mDataValue.number.UInteger64;		break;
	}
	return *this;
}

} // namespace ASN1
