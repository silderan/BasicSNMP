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

#ifndef SNMPTABLE_H
#define SNMPTABLE_H

#include "oid.h"
#include "snmpencoder.h"
#include "pduvarbind.h"
#include "stddeque.h"

#ifdef QT_CORE_LIB
#include <QString>
#endif

namespace SNMP {

/*
 * A table row in SNMP is coded like that into the OIDs:
 * <BaseOID>.<Column>.<IndexKeys>
 * For example:
 * BaseOID   = 1.3.1.2.3.4.
 * Column    = 1
 * IndexKeys = 7.8
 *
 * Result OID = 1.3.1.2.3.4.1.7.8
 *
 * Note that the official name is "indexes", but I prefer "index key" or just "key"
 * because its closer to what actual DDBB names it and also won't be misleaded with
 * actual "column indexes"
 *
 * There must be a special column called StatusRow (in SMIv2) or EntryStatus (in SMIv1)
 * used for manipulating rows.
 * When creating new row, sending messages 1 by 1, the first one must be this special column.
 * If sending all columns in the same SNMP message datagram (multiple Varbinds) agent should
 * take care if this and search for "Status" column first. But shall fail. So this
 * API will put StatusRow/EntryStatus column at the very begining.
 */

enum SMIVersion
{
	SMIUnd = -1,	// Undefined.
	SMIv1 = 1,
	SMIv2
};

// Control Column for SMIv1
enum EntryStatus
{
	valid = 1,
	createRequest,
	underCreation,
	invalid
};

// Control Column for SMIv2
enum StatusRow
{
	active = 1,
	notInService,
	notReady,
	createAndGo,
	createAndWait,
	destroy
};

struct TableInfo
{
	OID keyIndexes;
	UInt64 column;
	PDUVarbind varbind;

	TableInfo(const OID &baseOID, const Encoder &snmp)
	{
		interpret(baseOID, snmp);
	}
	void interpret(const OID &baseOID, const Encoder &snmp)
	{
		if( snmp.varbindList().count() )
		{
			varbind = snmp.varbindList().first();
			column = varbind.oid()[baseOID.count()].toULongLong();

			for( Int64 i = baseOID.count()+1; i < varbind.oid().count(); ++i )
				keyIndexes.push_back( varbind.oid()[i] );
		}
	}
};

// This two classes below are for known column sized tables.
// It's much more easy to code and use this ones.

// TableRow class stores one specific row cells data.
// One row include the keys and regular columns data.
// But, for the point of view of a standard table, it's just a cell vector.
// The count of keys and columns must be known before using it.
// Table class inmediatly resizes the keys and column vectors just after it's been created.
class TableRow : public StdVector<ASN1Variable>
{

public:
	void setCell(Int64 colIndex, const ASN1Variable &asn1Var)	{ cell(colIndex) = asn1Var; }
	void setCell(Int64 colIndex, const OIDValue &oidValue)		{ cell(colIndex).setUnsigned64(oidValue.toULongLong()); }

	template <typename T>
	const ASN1Variable &cell(T i) const					{ return at(static_cast<Int64>(i));	}
	const ASN1Variable &cell(const OIDValue &oid) const	{ return cell(oid.toULongLong());	}

	template <typename T>
	ASN1Variable &cell(T i)					{ return at(static_cast<Int64>(i));		}
	ASN1Variable &cell(const OIDValue &oid)	{ return cell(oid.toULongLong());		}
};

class Table : public StdDeque<TableRow>
{
	OID mBaseOID;
	Int64 mKeyCount;
	Int64 mColCount;
	Int64 mStatusColumnIndex;

public:
	Table(const OID &baseOID,
		  Int64 keyCount,
		  Int64 colCount,
		  Int64 statusColumnIndex = -1)
		: mBaseOID(baseOID)
		, mKeyCount(keyCount)
		, mColCount(colCount)
		, mStatusColumnIndex(statusColumnIndex)
	{
	}
	Int64 statusColumnIndex() const		{ return mStatusColumnIndex;	}
	Int64 keyCount() const				{ return mKeyCount;				}
	Int64 columnCount() const			{ return mColCount;				}

	// Beware. Never call this functions if statusColumnIndex is not set properly.
	const ASN1Variable &statusCell(Int64 col) const	{ return cell(col, statusColumnIndex());	}
	ASN1Variable &statusCell(Int64 col)				{ return cell(col, statusColumnIndex());	}

	const ASN1Variable &cell(Int64 row, Int64 col) const		{ return rowAt(row).cell(col);	}
	ASN1Variable &cell(Int64 row, Int64 col)					{ return rowAt(row).cell(col);	}
	void setCell(Int64 row, Int64 col, const ASN1Variable &var)	{ rowAt(row).cell(col) = var;	}

	const OID &baseOID() const		{ return mBaseOID;	}

	// Returns the row index giving the oid of the shell.
	// That is usefull when new data comes from an agent (varbind)
	// this funcion gets the keys from the OID and search for the
	// row having this exact keys.
	// Returns -1 if no found.
	// It's used also intercally in the addCell function.
	Int64 rowOf(const OID &cellOID) const;
	Int64 rowCount() const		{ return Table::count();		}

	// Those 6 following are a convenient ones for a more readable code.

	const TableRow &firstRow() const		{ return Table::first();		}
	TableRow &firstRow()					{ return Table::first();		}

	const TableRow &lastRow() const			{ return Table::last();			}
	TableRow &lastRow()						{ return Table::last();			}

	const TableRow &rowAt(Int64 i) const	{ return Table::at(i);	}
	TableRow &rowAt(Int64 i)				{ return Table::at(i);	}

	// Returns the index in the cell OID where the conceptual table column is stored.
	//                     Base OID             C     Keys
	//          ------------------------------- - ------------
	// OID   :  1.3.6.1.4.1.33369.1.1.1.1.1.4.1.2.346172.26276
	// Index :  0 1 2 3 4 5 6 7 8 9 A B C D E F H   I      J
	//                                          ^
	// It is allways just after the base OID.
	Int64 oidColumnIndex() const
	{
		return mBaseOID.count();
	}

	// Returns the index in the cell OID where the conceptual column keys are.
	//                     Base OID             C     Keys
	//          ------------------------------- - ------------
	// OID   :  1.3.6.1.4.1.33369.1.1.1.1.1.4.1.2.346172.26276
	// Index :  0 1 2 3 4 5 6 7 8 9 A B C D E F H   I      J
	//                                              ^      ^
	// They are after the column index.
	// keyIndex parameter is the 0-relative index of the local key vector.
	// i.e if there are two keys, must be 0 or 1 (less than keyCount())
	Int64 oidKeyIndex(Int64 keyIndex) const
	{
		return oidColumnIndex() + 1 + keyIndex;
	}
	// Returns the conceptual table column index.
	// It's stored as a value in the cell OID.
	//                     Base OID             C     Keys
	//          ------------------------------- - ------------
	// OID   :  1.3.6.1.4.1.33369.1.1.1.1.1.4.1.2.346172.26276
	//                                          ^
	Int64 columnIndex(const OID &cellOID) const
	{
		return static_cast<Int64>(cellOID.at(oidColumnIndex()).toULongLong());
	}

	// Returns the conceptual table key value.
	// It's stored as a value in the cell OID.
	//                     Base OID             C     Keys
	//          ------------------------------- - ------------
	// OID   :  1.3.6.1.4.1.33369.1.1.1.1.1.4.1.2.346172.26276
	//                                               ^     ^
	// keyIndex parameter is the 0-relative index of the local key vector.
	// i.e if there are two keys, must be 0 or 1 (less than keyCount())
	const ASN1Variable &keyValue(Int64 row, Int64 keyIndex) const
	{
		return rowAt(row).cell(keyIndex);
	}

	// Returns the keys value of the row.
	// It's essentialy an array of keyValue()
	StdDeque<ASN1Variable> keys(Int64 row) const;

	// Adds a cell with the ASN1Variant data into the table.
	// Adds new rows if needed.
	// Puts in row and col variable the indexes where the cell is created.
	bool addCell(const PDUVarbind &pduVarbind, Int64 &row, Int64 &col);

	// Returns the OID for the cell identified by the row and conceptual column index.
	// This OID can be used to request specific cell data to/from agent.
	OID cellOID(Int64 row, Int64 col) const
	{
		OID oid;
		oid.reserve( baseOID().count() + 1 + keyCount() );
		oid.append( baseOID() );
		oid.append( OIDValue(col) );
		for( const ASN1Variable &var : keys(row) )
			oid.append( OIDValue(var.toUnsigned64()) );
		return oid;
	}
};

}	// namespace SNMP

#endif // SNMPTABLE_H
