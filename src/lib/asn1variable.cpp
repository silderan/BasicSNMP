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
