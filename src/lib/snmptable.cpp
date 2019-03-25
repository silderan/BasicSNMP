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

#include "snmptable.h"

using namespace SNMP;

bool TableRow::setColumn(Int64 colIndex, const ASN1Variable &asn1Var)
{
	if( (colIndex >= 0) && (colIndex < mColumns.count()) )
	{
		column(colIndex) = asn1Var;
		return true;
	}
	return false;
}

bool TableRow::setKey(Int64 keyIndex, const OIDValue &oidValue)
{
	if( (keyIndex >= 0) && (keyIndex < mKeys.count()) )
	{
		key(keyIndex) = oidValue;
		return true;
	}
	return false;
}

