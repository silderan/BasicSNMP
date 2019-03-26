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

StdDeque<ASN1Variable> Table::keys(Int64 row) const
{
	StdDeque<ASN1Variable> rtn;
	for( int k = 0; k < keyCount(); ++k )
		rtn.append( keyValue(row, k) );
	return rtn;
}

Int64 Table::rowOf(const OID &cellOID) const
{
	for( Int64 row = 0; row < Table::count(); ++row )
	{
		for( Int64 k = 0; k < keyCount(); ++ k )
			if( rowAt(row).at(k).toUnsigned64() == cellOID.at(oidKeyIndex(k)).toULongLong() )
				return row;
	}
	return -1;
}

bool Table::addCell(const PDUVarbind &pduVarbind, Int64 &row, Int64 &col)
{
	OID oid = pduVarbind.oid();
	if( oid.startsWith(baseOID()) )
	{
		row = rowOf(oid);
		if( row == -1 )
		{
			row = Table::count();
			// Add a row.
			append( TableRow() );
			lastRow().resize( columnCount() );
			// set keys.
			for( Int64 k = 0; k < keyCount(); ++k )
				lastRow().setCell(k, oid.at(oidKeyIndex(k)));
		}
		// Set column data.
		// This col index is 1-based.
		// Also, actual column index is moved by the keys
		// So, if the columnIndex == 2 and there are 4 keys.
		// the correct column must be 2-1+4 = 5
		col = columnIndex(oid)-1+keyCount();
		rowAt(row).setCell( col, pduVarbind );
		return true;
	}
	return false;
}
