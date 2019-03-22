#ifndef QCONSTANTSSTRINGS_H
#define QCONSTANTSSTRINGS_H

#include <QMap>
#include <QString>

#include "lib/snmplib.h"

namespace SNMPConstants {

const QMap<ASN1::DataType,QString> &asn1TypeMap();
QString asn1TypeName(ASN1::DataType type);

QString asn1PrintableValue(const ASN1::Variable &asn1Var);

template <typename V>
QString printableRawData(const V &v)
{
	return QString::fromStdString(Utils::printableBytes(v, " "));
}

inline QString printableRawData(const SNMP::PDUVarbind &varbind)
{
	return QString::fromStdString(Utils::printableBytes(varbind.rawValue(), " "));
}

QString printableErrorCode(ASN1::Encoder::ErrorCode ec);
QString printableErrorCode(const SNMP::Encoder &snmp);

};
#endif // QCONSTANTSSTRINGS_H
