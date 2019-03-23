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

#ifndef STDCHARVECTOR_H
#define STDCHARVECTOR_H

#include <vector>
#include <string>

#ifdef QT_CORE_LIB
#include <QString>
#include <QByteArray>
#endif

#include "utils.h"

class StdCharVector : public std::vector<char>
{

public:
	StdCharVector() = default;

	StdCharVector(const std::vector<char> &v)
		: std::vector<char> (v)
	{	}

	StdCharVector(const char *a, long long len)
		: StdCharVector(len)
	{
		while( len-- )
			at(len) = a[len];
	}
	StdCharVector(const std::string &s)
		: StdCharVector(s.data(), static_cast<long long>(s.size()) )
	{	}
	StdCharVector(long long t)
		: std::vector<char>( static_cast<unsigned long long>(t) )
	{	}
	char &at(long long i)
	{
		return std::vector<char>::at( static_cast<std::vector<char>::size_type >(i)) ;
	}
	char at(long long i) const
	{
		return std::vector<char>::at( static_cast<std::vector<char>::size_type >(i)) ;
	}
	char operator[](long long i) const	{ return at(i);	}
	char &operator[](long long i)		{ return at(i);	}
	bool isEmpty() const	{ return count() == 0;	}

	bool contains(char c) const
	{
		return Utils::contains(*this, c);
	}
	long long count() const		{ return static_cast<long long>(size());	}
	void resize(long long s)	{ std::vector<char>::resize(static_cast<std::vector<char>::size_type>(s));	}
	void insert(long long pos, char c)
	{
		std::vector<char>::insert( std::begin(*this) + static_cast<StdCharVector::difference_type>(pos), c );
	}
	void insert(long long pos, const StdCharVector &v)
	{
		std::vector<char>::insert( std::begin(*this)+pos, std::begin(v), std::end(v) );
	}
	void insert(long long pos, const char *c)
	{
		insert( pos, StdCharVector(c, static_cast<long long>(strlen(c))) );
	}
	StdCharVector mid( long long pos, long long length ) const
	{
		if( (pos + length) > count() )
			length = static_cast<unsigned int>(count() - pos);
		StdCharVector rtn(length);
		while( length-- > 0 )
			rtn[length] = at(pos+length);

		return rtn;
	}
	std::string printableBytes(const std::string &sep = "")const
	{
		return Utils::printableBytes(*this, sep);
	}
	StdCharVector &append(char c)
	{
		push_back(c);
		return *this;
	}

	StdCharVector &append(const StdCharVector &b)
	{
		static_cast<std::vector<char> &>(*this).insert( std::end(*this), std::begin(b), std::end(b) );
		return *this;
	}
	StdCharVector &append(const char *b)
	{
		return append( StdCharVector(b, static_cast<long long>(strlen(b))) );
	}
	StdCharVector &operator +=(const StdCharVector &b)
	{
		return append(b);
	}
#ifdef QT_CORE_LIB
	StdCharVector(const QByteArray &ba)
		: StdCharVector(ba.constData(), ba.count())
	{	}
	StdCharVector(const QString &s)
		: StdCharVector(s.toLatin1())
	{	}

	QString qPrintableBytes(const QString &sep = "")
	{
		return QString::fromStdString(Utils::printableBytes(*this, sep.toStdString()));
	}
#endif
};

#endif // STDCHARVECTOR_H
