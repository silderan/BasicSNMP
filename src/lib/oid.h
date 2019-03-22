#ifndef OID_H
#define OID_H

#include <utils.h>
#include <string>

#include "stdvector.h"

#ifdef QT_CORE_LIB
#include <QString>
#endif

class OIDValue
{
	std::string mStringValue;
	unsigned long long mValue;

public:
	OIDValue()
	{	}
	OIDValue(const char *v)
		: mStringValue( v )
		, mValue( std::stoull(v) )
	{	}
	OIDValue(const std::string &s)
		: OIDValue( s.data() )
	{	}
	template <typename T>
	OIDValue(T v)
		: mValue( static_cast<unsigned long long>(v) )
	{	}

	std::string toStdString() const
	{
		if( mStringValue.length() != 0 )
			return mStringValue;
		return std::to_string(mValue);
	}
	unsigned long long toULongLong() const
	{
		return mValue;
	}
	template <typename T>
	bool operator ==(T v) const	{ return mValue == static_cast<unsigned long long>(v);	}
	template <typename T>
	bool operator !=(T v) const	{ return mValue != static_cast<unsigned long long>(v);	}
	bool operator ==(const std::string &v) const	{ return std::stoull(v) == mValue;	}
	bool operator !=(const std::string &v) const	{ return std::stoull(v) != mValue;	}
	bool operator ==(const OIDValue &v) const		{ return v.mValue == mValue;		}
	bool operator !=(const OIDValue &v) const		{ return v.mValue != mValue;		}

#ifdef QT_CORE_LIB
	OIDValue( const QString &v )
		: OIDValue( v.toStdString() )
	{	}
	bool operator ==(const QString &v) const	{ return v.toULongLong() == mValue;		}
	bool operator !=(const QString &v) const	{ return v.toULongLong() != mValue;		}
	QString toQString() const
	{
		return QString::fromStdString(toStdString());
	}
#endif
};

class OID : public StdVector<OIDValue>
{
public:
	OID() = default;
	OID( long long initialSize )
		: StdVector(initialSize)
	{	}
	OID( int initialSize )
		: StdVector(initialSize)
	{	}
	OID( const StdVector<std::string> &oid )
		: StdVector<OIDValue>(oid.count())
	{
		long long i = 0;
		for( std::string bit : oid )
			at(i++) = bit;
	}
	OID( const char *charStr )
		: OID( Utils::split( std::string(charStr), '.') )
	{	}
	OID( const std::string &s )
		: OID( Utils::split(s, '.') )
	{	}
	OID( const OIDValue &a )
		: OID(1ll)
	{
		at(0) = a;
	}
	bool startsWith(const OID &oid) const
	{
		if( oid.count() > count() )
			return false;
		for( int i = 0; i < oid.count(); ++i )
			if( oid.at(i) != at(i) )
				return false;
		return true;
	}
	std::string toStdString() const
	{
		std::string rtn;
		for( OIDValue val : *this )
			rtn += "." +  val.toStdString();
		return rtn;
	}
#ifdef QT_CORE_LIB
	OID( const QString &str )
		: OID( str.toStdString() )
	{
	}
	QString toQString() const
	{
		return QString::fromStdString( toStdString() );
	}
#endif
};
typedef StdVector<OID> OIDList;

#endif // OID_H
