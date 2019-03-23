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

namespace SNMP {

const std::map<EntryStatus, StdString> &entryStatusInfoMap()
{
	static std::map<EntryStatus, StdString> infoMap =
	{
		{EntryStatus::valid,			"EntryStatus: valid"},
		{EntryStatus::createRequest,	"EntryStatus: createRequest"},
		{EntryStatus::underCreation,	"EntryStatus: underCreation"},
		{EntryStatus::invalid,			"EntryStatus: invalid"}
	};
	return infoMap;
}

StdString entryStatusName(EntryStatus es)
{
	return Utils::value( entryStatusInfoMap(), es, StdString("EntryStatus: error-code") );
}

const std::map<StatusRow, StdString> &statusRowInfoMap()
{
	static std::map<StatusRow, StdString> infoMap =
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
StdString statusRowName(StatusRow sr)
{
	return Utils::value( statusRowInfoMap(), sr, StdString("EntryStatus: error-code") );
}

} // namespace SNMP
