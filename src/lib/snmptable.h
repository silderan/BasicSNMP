#ifndef SNMPTABLE_H
#define SNMPTABLE_H

#include "oid.h"
#include "snmpencoder.h"
#include "pduvarbind.h"

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

const std::map<EntryStatus, std::string> &entryStatusInfoMap();
std::string entryStatusName(EntryStatus es);
#ifdef QT_CORE_LIB
inline QString qEntryStatusName(EntryStatus es)
{
	return QString::fromStdString(entryStatusName(es));
}
#endif

const std::map<StatusRow, std::string> &statusRowInfoMap();
std::string statusRowName(StatusRow sr);
#ifdef QT_CORE_LIB
inline QString qStatusRowName(StatusRow sr)
{
	return QString::fromStdString(statusRowName(sr));
}
#endif

struct TableInfo
{
	OID keyIndexes;
	unsigned long long column;
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

			for( long long i = baseOID.count()+1; i < varbind.oid().count(); ++i )
				keyIndexes.push_back( varbind.oid()[i] );
		}
	}
};

}	// namespace SNMP

#endif // SNMPTABLE_H
