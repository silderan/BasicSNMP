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

#ifndef QCONSTANTSSTRINGS_H
#define QCONSTANTSSTRINGS_H

#include <QMap>
#include <QString>

#include "lib/snmplib.h"
#include "lib/snmptable.h"

namespace SNMPConstants {

const QMap<SNMP::ASN1DataType,QString> &asn1TypeMap();
QString asn1TypeName(SNMP::ASN1DataType type);

QString asn1PrintableValue(const SNMP::ASN1Variable &asn1Var);

template <typename V>
QString printableRawData(const V &v)
{
	return QString::fromStdString(SNMP::Utils::printableBytes(v, " "));
}

inline QString printableRawData(const SNMP::PDUVarbind &varbind)
{
	return QString::fromStdString(SNMP::Utils::printableBytes(varbind.rawValue(), " "));
}

QString printableErrorCode(SNMP::ASN1Encoder::ErrorCode ec);
QString printableErrorCode(const SNMP::Encoder &snmp);

const QMap<SNMP::EntryStatus, QString> &entryStatusInfoMap();
QString entryStatusName(SNMP::EntryStatus es);

const QMap<SNMP::StatusRow, QString> &statusRowInfoMap();
QString statusRowName(SNMP::StatusRow sr);


};
#endif // QCONSTANTSSTRINGS_H
