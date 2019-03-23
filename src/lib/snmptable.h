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

namespace SNMP
{

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
	SNMP::PDUVarbind varbind;

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

// TableRow class stores information of one specific row.
// One row include the keys and regular columns data.
// The count of keys and columns must be known before creating it.
// Derived classes must provide such data.
class TableRow
{
	StdVector<ASN1::Variable> mColumns;
	OID mKeys;

public:
	TableRow(Int64 columnCount, Int64 keyCount);
	bool setColumn(Int64 colIndex, const ASN1::Variable &asn1Var);
	bool setKey(Int64 keyIndex, const OIDValue &oidValue);

	template <typename T>
	const ASN1::Variable &column(T i) const					{ return mColumns.at(static_cast<Int64>(i));	}
	const ASN1::Variable &column(const OIDValue oid) const	{ return column(oid.toULongLong());	}

	template <typename T>
	ASN1::Variable &column(T i)					{ return mColumns[static_cast<int>(i)];	}
	ASN1::Variable &column(const OIDValue oid)	{ return column(oid.toULongLong());		}

	const OIDValue &key(Int64 i)const 		{ return mKeys[i];	}
	OIDValue &key(Int64 i)					{ return mKeys[i];	}

	bool matchKey(const OID &oid) const
	{
		return oid.endsWith(mKeys);
	}
};

/* T in this class must be a subclass of SNMP::TableRow
   But, One derived constructor must require exactly 2 parameters because it's spected like that in addCell funcion.
   Derived class must itself initializate SNMP::TableRow with both column and key count.
   Like that:
   class DerivedRow : public SNMP::TableRow
   {
   public:
		DerivedRow(int colCount, int keyCount)
			: SNMP::TableRow(colCount, keyCount)
			{	}
   }
*/
template <class T>
class Table : public StdDeque<T>
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
	const ASN1::Variable &statusCell(Int64 col) const	{ return columnCell(col, statusColumnIndex());	}
	ASN1::Variable &statusCell(Int64 col)				{ return columnCell(col, statusColumnIndex());	}

	const ASN1::Variable &columnCell(Int64 row, Int64 col) const	{ return Table::operator[](row).column(col);	}
	ASN1::Variable &columnCell(Int64 row, Int64 col)				{ return Table::operator[](row).column(col);	}

	const ASN1::Variable &keyCell(Int64 row, Int64 key) const	{ return Table::operator[](row).key(key);	}
	ASN1::Variable &keyCell(Int64 row, Int64 key)				{ return Table::operator[](row).key(key);	}

	const OID &baseOID() const	{ return mBaseOID;	}
	int indexOf(const OID &keyOID) const
	{
		for( int i = 0; i < Table::count(); ++i )
		{
			if( Table::operator[](i).matchKey(keyOID) )
				return i;
		}
		return -1;
	}
	void addCell(const SNMP::PDUVarbind &pduVarbind)
	{
		OID oid = pduVarbind.oid();
		int i = indexOf(oid);
		if( i == -1 )
		{
			i = Table::count();
			// Add a row.
			Table::append( T(mKeyCount, mColCount) );
			// set keys.
			for( int k = 0; k < keyCount(); ++k )
			{
				OIDValue keyOID = oid.at(columnCount()+1+k);
				Table::operator[](i).setKey(k, keyOID);
			}
		}
		OIDValue columnOID = oid.at( columnCount() );
		Table::operator[](i).setColumn( columnOID.toULongLong()-1, pduVarbind );
	}
	void addCells(const SNMP::PDUVarbindList &pduVarbindList)
	{
		for( const SNMP::PDUVarbind &var : pduVarbindList )
			addCell(var);
	}
	void addCells(const SNMP::Encoder &snmp)
	{
		addCells(snmp.varbindList());
	}
};

}	// namespace SNMP

#endif // SNMPTABLE_H
