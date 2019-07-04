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

#include <random>
#include <ctime>
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

template <typename T>
class TableBaseInfo
{
	static OID mOIDBase;
	static Int64 mKeyCount;
	static Int64 mFirstColumn;
	static Int64 mLastColumn;
	static Int64 mOIDColumnIndex;	// This is the index in the OID array where the column is. Usually, it is the very next to the OID base.
	int mDataId;

public:
	TableBaseInfo(int dataId)
		: mDataId(dataId)
	{	}
	static Int64 keyCount()			{ return mKeyCount;			}
	static Int64 firstColumn()		{ return mFirstColumn;		}
	static Int64 lastColumn() 		{ return mLastColumn;		}
	static const OID &oidBase()		{ return mOIDBase;			}
	static Int64 oidColumnIndex()	{ return mOIDColumnIndex;	}
	static OIDValue oidColumnValue(const OID &oid )	{ return oid.at(oidColumnIndex());	}

	static Int64 oidFirstKeyIndex()			{ return oidColumnIndex()+1;	}
	static Int64 oidKeyIndex(int keyIndex)	{ return oidFirstKeyIndex()+keyIndex;	}
	static OIDValue oidKeyValue(const OID &oid, int keyIndex) { return oid.at(oidKeyIndex(keyIndex));	}

	static Int64 columnCount()		{ return (lastColumn() - firstColumn()) + 1; }
	int dataId() const { return mDataId;	}
};

template <typename T>
class TableRowBase : public TableBaseInfo<T>
{
	StdVector<ASN1Variable> mCells;
	OID mKeys;

public:
	TableRowBase( int dataId )
		: TableBaseInfo<T> (dataId)
		, mCells( TableRowBase::columnCount() )
		, mKeys( TableRowBase::keyCount() )
	{	}

	Int64 columnToIndex(Int64 column) const		{ return column - TableRowBase::firstColumn();	}
	Int64 indexToColumn(Int64 index) const		{ return index + TableRowBase::firstColumn();	}

	bool machKeys(const OID &oid) const
	{
		return  oid.endsWith(mKeys);
	}

	const OID &keys() const				{ return mKeys;			}
	const OIDValue &key(Int64 i) const	{ return mKeys.at(i);	}
	OIDValue &key(Int64 i)				{ return mKeys.at(i);	}

	OID cellOID(Int64 col) const
	{
		OID oid;
		oid.reserve( TableRowBase::oidBase().count() + 1 + TableRowBase::keyCount() );
		oid.append( TableRowBase::oidBase() );
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

		for( Int64 col = TableRowBase::firstColumn(); col <= TableRowBase::lastColumn(); ++col )
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
class TableBase : public StdDeque<T>, public TableBaseInfo<T>
{

public:
	TableBase(int dataId)
		: TableBaseInfo<T>(dataId)
	{	}

	Int64 rowOf(const OID &oid) const
	{
		for( Int64 row = 0; row < TableBase::count(); ++row )
		{
			if( TableBase::at(row).machKeys(oid) )
				return row;
		}
		return -1;
	}
	Int64 rowOf(const PDUVarbind &varBind) const
	{
		if( varBind.oid().startsWith(TableBase::oidBase()) )
			return rowOf( varBind.oid() );
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
		if( varBind.oid().startsWith(TableBase::oidBase()) )
		{
			Int64 col = static_cast<Int64>(varBind.oid().at(TableBase::oidBase().count()).toULongLong());
			if( col < TableBase::firstColumn() )
				std::cerr << __func__ << " column " << col << ", in the OID " << varBind.oid().toStdString() << ", is less than the first configured: " << TableBase::firstColumn() << std::endl;
			else
			if( col > TableBase::lastColumn() )
				std::cerr << __func__ << " column " << col << ", in the OID " << varBind.oid().toStdString() << ", is greater than the last configured: " << TableBase::lastColumn() << std::endl;
			else
			{
				Int64 row = rowOf( varBind.oid() );

				if( row == -1 )
				{
					row = TableBase::count();
					TableBase::append( T(TableBase::dataId()) );

					// Copy the keys.
					for( int key= 0; key < TableBase::keyCount(); ++key )
						TableBase::last().key(key) = TableBase::oidKeyValue( varBind.oid(), key );
				}
				TableBase::at(row).cell(col) = varBind.asn1Variable();
				return row;
			}
		}
		return -1;
	}
	void removeRow(Int64 row)
	{
		if( (row >= 0) && (row < TableBase::count()) )
			TableBase::removeAt(row);
	}
	void removeRow(const PDUVarbind &varBind)
	{
		removeRow( rowOf(varBind) );
	}
	// Hey, you. It's not a good idea to add functions that accepts
	// snmp or varbindlist data because it may include non related OIDs
	// and, for be sure that the incoming data is for the actual table,
	// is imperative that you use a the above function using PDUVarbind

	// Returns key row of the key passed..
	Int64 keyRow( Int64 keyindex, const OIDValue &oid ) const
	{
		assert(keyindex < TableBase::keyCount());
		for( int row = 0; row < TableBase::count(); ++row )
		{
			if( TableBase::at(row).key(keyindex) == oid )
				return row;
		}
		return -1;
	}
	// Finds a new key for the key row index.
	// It's usefull for new table intries
	template<typename Num = UInt64>
	Num newSequencialKeyRowValue( Int64 keyIndex, Num initialNumber = 1 ) const
	{
		UInt64 id = static_cast<UInt64>(initialNumber);

		while( keyRow(keyIndex, SNMP::OIDValue(id)) != -1 )
			++id;

		return id;
	}
	// Finds a new key for the key row index.
	// It's usefull for new table intries
	template<typename Num = UInt64>
	Num newRandomKeyRowValue( Int64 keyIndex, Num min = 1, Num max = -1 ) const
	{
		UInt64 id = static_cast<UInt64>(min);

		static std::mt19937 gen(std::time(nullptr));
		std::uniform_int_distribution<Num> dis(min, max);

		do
		{
			id = static_cast<UInt64>( dis(gen) );
		}
		while( keyRow(keyIndex, SNMP::OIDValue(id)) != -1 );

		return id;
	}
	// Returns the row index where the comun' cell has the value passed.
	// Be sure before calling that column stores number values.
	int findCellRow( Int64 column, Int64 value ) const
	{
		for( int row = 0; row < TableBase::count(); ++row )
		{
			if( TableBase::at(row).cell(column).toInteger() == value )
				return row;
		}
		return -1;
	}
	// Returns a new data for the column.
	// It's usefull for new table entries where the entry must be unique per column.
	Int64 newCellRowValue( Int64 column ) const
	{
		Int64 id = 1;

		while( findCellRow(column, id) != -1 )
			++id;

		return id;
	}
	// Returns the row index where the comun' cell has the value passed.
	// Be sure before calling that column stores string-type values (octet string, and so on).
	int findCellRow( Int64 column, const StdString &value ) const
	{
		for( int row = 0; row < TableBase::count(); ++row )
		{
			if( TableBase::at(row).cell(column).toStdString() == value )
				return row;
		}
		return -1;
	}
	// Returns a new data for the column.
	// It's usefull for new table entries where the entry must be unique per column.
	template<typename Num = Int64>
	StdString newCellRowValue( Int64 column, const StdString &prefix, Num initial = 1 ) const
	{
		Int64 id = static_cast<Int64>(initial);

		while( findCellRow(column, prefix + std::to_string(id)) != -1 )
			++id;

		return prefix + std::to_string(id);
	}
};

}	// namespace SNMP

#endif // SNMPTABLE_H
