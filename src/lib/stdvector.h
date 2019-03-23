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

#ifndef STDVECTOR_H
#define STDVECTOR_H

#include <vector>

template<typename T>
class StdVector : public std::vector<T>
{
public:
	StdVector() = default;
	StdVector( const std::vector<T> &v)
		: std::vector<T> (v)
	{	}
	StdVector( typename std::vector<T>::size_type s )
		: std::vector<T> (s)
	{	}
	StdVector( long long initialSize )
		: std::vector<T> (initialSize)
	{	}
	StdVector( int initialSize )
		: std::vector<T> (initialSize)
	{	}
	StdVector( const T &t )
		: std::vector<T>(1l)
	{
		at(0) = t;
	}
	StdVector( const T &t0, const T &t1 )
		: std::vector<T>(2l)
	{
		at(0) = t0;
		at(1) = t1;
	}
	StdVector( const T &t0, const T &t1, const T &t2 )
		: std::vector<T>(3l)
	{
		at(0) = t0;
		at(1) = t1;
		at(2) = t2;
	}
	StdVector( const StdVector<T> &v )
		: std::vector<T> (v)
	{	}
//	StdVector( const StdVector<T> &v )
//		: std::vector<T>(static_cast<long long>(v.size()))
//	{
//		for( typename StdVector<T>::size_type i = 0; i < v.size(); ++i )
//			at(i) = v.at(i);
//	}
	long long count()	const	{ return static_cast<long long>(std::vector<T>::size());	}
	void resize(long long i)	{ std::vector<T>::resize(static_cast<typename std::vector<T>::size_type>(i));	}

	void append(const T &t)					{ return this->push_back(t);	}
	void append(const StdVector<T> &t)		{ return this->push_back(t);	}

	const T &operator[](long long i) const	{ return std::vector<T>::operator[](static_cast<typename std::vector<T>::size_type>(i));	}
	T &operator[](long long i)				{ return std::vector<T>::operator[](static_cast<typename std::vector<T>::size_type>(i));	}
	const T &at(long long i) const	{ return std::vector<T>::at(static_cast<typename std::vector<T>::size_type>(i));	}
	T &at(long long i)				{ return std::vector<T>::at(static_cast<typename std::vector<T>::size_type>(i));	}

	bool isEmpty() const	{ return count() == 0;	}
};

#endif // STDVECTOR_H
