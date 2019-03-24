/**************************************************************************

  Copyright 2015-2019 Rafael Dellà Bort. silderan (at) gmail (dot) com

  This file is part of BasicSNMP

  BasicSNMP is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as
  published by the Free Software Foundation, either version 3 of
  the License, or (at your option) any later version.

  QMikPPPManager is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  and GNU Lesser General Public License. along with QMikPPPManager.
  If not, see <http://www.gnu.org/licenses/>.

**************************************************************************/
#ifndef BASIC_SNMP_BASIC_TYPES_H
#define BASIC_SNMP_BASIC_TYPES_H

namespace SNMP {

typedef char Int8;
typedef short Int16;
typedef int	Int32;
typedef long long Int64;

typedef unsigned char Byte;
typedef Byte UInt8;
typedef unsigned short UInt16;
typedef unsigned int UInt32;
typedef unsigned long long UInt64;

} // namespace SNMP

#endif // BASIC_SNMP_BASIC_TYPES_H
