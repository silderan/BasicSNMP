#include "snmptable.h"

namespace SNMP {

const std::map<EntryStatus, std::string> &entryStatusInfoMap()
{
	static std::map<EntryStatus, std::string> infoMap =
	{
		{EntryStatus::valid,			"EntryStatus: valid"},
		{EntryStatus::createRequest,	"EntryStatus: createRequest"},
		{EntryStatus::underCreation,	"EntryStatus: underCreation"},
		{EntryStatus::invalid,			"EntryStatus: invalid"}
	};
	return infoMap;
}

std::string entryStatusName(EntryStatus es)
{
	return Utils::value( entryStatusInfoMap(), es, std::string("EntryStatus: error-code") );
}

const std::map<StatusRow, std::string> &statusRowInfoMap()
{
	static std::map<StatusRow, std::string> infoMap =
	{
		{StatusRow::active,			"EntryStatus: valid"},
		{StatusRow::notInService,	"EntryStatus: notInService"},
		{StatusRow::notReady,		"EntryStatus: notReady"},
		{StatusRow::createAndGo,	"EntryStatus: createAndGo"},
		{StatusRow::createAndWait,	"EntryStatus: createAndWait"},
		{StatusRow::destroy,		"EntryStatus: destroy"}
	};
	return infoMap;
}
std::string statusRowName(StatusRow sr)
{
	return Utils::value( statusRowInfoMap(), sr, std::string("EntryStatus: error-code") );
}

} // namespace SNMP
