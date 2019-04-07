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

#ifndef QSNMPLIB_H
#define QSNMPLIB_H

namespace SNMP {
	enum Version
	{
		V1 = 0,
		V2 = 1
	};
}
#include "types.h"
#include "../utils.h"
#include "stdcharvector.h"
#include "stdlist.h"
#include "stddeque.h"
#include "asn1types.h"
#include "oid.h"
#include "asn1variable.h"
#include "asn1encoder.h"
#include "pduvarbind.h"
#include "snmpencoder.h"
#include "snmptable.h"


#endif // QSNMPLIB_H
