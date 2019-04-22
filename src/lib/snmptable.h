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

#include <iostream>

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
enum StatusEntry
{
	valid = 1,
	createRequest,
	underCreation,
	invalid
};

// Control Column for SMIv2
enum RowStatus
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

class TableBaseInfo
{
	const OID &mOIDBase;
	Int64 mKeyCount;
	Int64 mFirstColumn;
	Int64 mLastColumn;
	Int64 mOIDColumnIndex;

public:
	TableBaseInfo(const OID &oidBase, Int64 keyCount, Int64 firstColumn, Int64 lastColumn, Int64 oidColumnIndex )
		: mOIDBase( oidBase )
		, mKeyCount( keyCount )
		, mFirstColumn( firstColumn )
		, mLastColumn( lastColumn )
		, mOIDColumnIndex( oidColumnIndex )
	{	}

	Int64 keyCount() const			{ return mKeyCount;			}
	Int64 firstColumn() const		{ return mFirstColumn;		}
	Int64 lastColumn() const		{ return mLastColumn;		}
	const OID &oidBase() const		{ return mOIDBase;			}
	Int64 oidColumnIndex() const	{ return mOIDColumnIndex;	}
	OIDValue oidColumnValue(const OID &oid )	{ return oid.at(oidColumnIndex());	}

	Int64 oidFirstKeyIndex() const			{ return oidColumnIndex()+1;	}
	Int64 oidKeyIndex(int keyIndex) const	{ return oidFirstKeyIndex()+keyIndex;	}
	OIDValue oidKeyValue(const OID &oid, int keyIndex) const{ return oid.at(oidKeyIndex(keyIndex));	}

	Int64 columnCount() const		{ return (lastColumn() - firstColumn()) + 1; }
};

class TableRowBase : public TableBaseInfo
{
	StdVector<ASN1Variable> mCells;
	OID mKeys;

public:
	TableRowBase(const OID &oidBase, Int64 keyCount, Int64 firstColumn, Int64 lastColumn, Int64 oidColumnIndex)
		: TableBaseInfo ( oidBase, keyCount, firstColumn, lastColumn, oidColumnIndex )
		, mCells( columnCount() )
		, mKeys( keyCount )
	{	}

	Int64 columnToIndex(Int64 column) const		{ return column - firstColumn();	}
	Int64 indexToColumn(Int64 index) const		{ return index + firstColumn();		}

	bool machKeys(const OID &oid)
	{
		return  oid.endsWith(mKeys);
	}

	const OID &keys() const				{ return mKeys;			}
	const OIDValue &key(Int64 i) const	{ return mKeys.at(i);	}
	OIDValue &key(Int64 i)				{ return mKeys.at(i);	}

	OID cellOID(Int64 col) const
	{
		OID oid;
		oid.reserve( oidBase().count() + 1 + keyCount() );
		oid.append( oidBase() );
		oid.append( OIDValue(col) );
		oid.append( keys() );

		return oid;
	}

	const ASN1Variable &cell(Int64 col) const	{ return mCells.at( columnToIndex(col) ); }
	ASN1Variable &cell(Int64 col)				{ return mCells.at( columnToIndex(col) ); }

	PDUVarbind varbind(Int64 col) const		{ return PDUVarbind( cellOID(col), cell(col) ); }
	PDUVarbindList varbindList(Int64 rowStatusCol) const
	{
		PDUVarbindList list;

		if( rowStatusCol != -1 )
			list.append( varbind(rowStatusCol) );

		for( Int64 col = firstColumn(); col <= lastColumn(); ++col )
			if( col != rowStatusCol )
				list.append( varbind(col) );

		return list;
	}
	template <class _list>
	PDUVarbindList varbindList(_list colList) const
	{
		PDUVarbindList list;

		for( auto col : colList )
			list.append( varbind(col) );

		return list;
	}
};

template <class T>
class TableBase : public StdDeque<T>, public TableBaseInfo
{
public:
	TableBase(const OID &oidBase, Int64 keyCount, Int64 firstColumn, Int64 lastColumn, Int64 oidColumnIndex)
		: TableBaseInfo ( oidBase, keyCount, firstColumn, lastColumn, oidColumnIndex )
	{	}

	Int64 indexOf(const OID &oid)
	{
		for( Int64 row = 0; row < TableBase::count(); ++row )
		{
			if( TableBase::at(row).machKeys(oid) )
				return row;
		}
		return -1;
	}

	// Sets cell data to the table.
	// This funcion checks the OID in the varbind to know if it
	// match to the table base OID.
	// Also, checks the keys (aka, indexes) in the oid to see
	// if is necessary to add a new row.
	// Returns the row col where cell is inserted or -1 if
	// the data is not for this table.
	Int64 setCellData(const PDUVarbind &varBind)
	{
		if( varBind.oid().startsWith(oidBase()) )
		{
			Int64 col = static_cast<Int64>(varBind.oid().at(oidBase().count()).toULongLong());
			if( col < firstColumn() )
				std::cerr << __func__ << " column " << col << ", in the OID " << varBind.oid().toStdString() << ", is less than the first configured: " << firstColumn() << std::endl;
			else
			if( col > lastColumn() )
				std::cerr << __func__ << " column " << col << ", in the OID " << varBind.oid().toStdString() << ", is greater than the last configured: " << lastColumn() << std::endl;
			else
			{
				Int64 row = indexOf( varBind.oid() );

				if( row == -1 )
				{
					row = TableBase::count();
					TableBase::append( T() );

					// Copy the keys.
					for( int key= 0; key < keyCount(); ++key )
						TableBase::last().key(key) = oidKeyValue( varBind.oid(), key );
				}
				TableBase::at(row).cell(col) = varBind.asn1Variable();
				return row;
			}
		}
		return -1;
	}
	// Hey, you. It's not a good idea to add functions that accepts
	// snmp or varbindlist data because it may include non related OIDs
	// and, for be sure that the incoming data is for the actual table,
	// is imperative that you use a the above function using PDUVarbind

	// Returns key row of the key passed..
	Int64 keyRow( Int64 keyindex, const OIDValue &oid ) const
	{
		assert(keyindex < keyCount());
		for( int row = 0; row < TableBase::count(); ++row )
		{
			if( TableBase::at(row).key(keyindex) == oid )
				return row;
		}
		return -1;
	}
	// Finds a new key for the key row index.
	// It's usefull for new table intries
	UInt64 newKeyRowValue( Int64 keyIndex ) const
	{
		UInt64 id = 1;

		while( keyRow(keyIndex, SNMP::OIDValue(id)) != -1 )
			++id;

		return id;
	}
	// Returns the row for the matched cell integer value.
	// Ensure that column stores number values.
	int cellRow( Int64 column, Int64 value ) const
	{
		for( int row = 0; row < TableBase::count(); ++row )
		{
			if( TableBase::at(row).cell(column).toInteger() == value )
				return row;
		}
		return -1;
	}
	// Finds a new data for the column.
	// It's usefull for new table entries.
	Int64 newCellRowValue( Int64 column ) const
	{
		Int64 id = 1;

		while( cellRow(column, id) != -1 )
			++id;

		return id;
	}
	// Returns the row for the matched cell string value.
	// Ensure that column stores string-type values (octet string, and so on).
	int cellRow( Int64 column, const StdString &value ) const
	{
		for( int row = 0; row < TableBase::count(); ++row )
		{
			if( TableBase::at(row).cell(column).toStdString() == value )
				return row;
		}
		return -1;
	}
};

}	// namespace SNMP

#endif // SNMPTABLE_H
