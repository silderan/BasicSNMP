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
#ifndef STDDEQUE_H
#define STDDEQUE_H

#include <deque>
#include <functional>

#include "basic_types.h"

namespace SNMP {
template <typename T>
class StdDeque : public std::deque<T>
{
public:
	StdDeque &append(const StdDeque<T> &t)	{ StdDeque<T>::push_back(t);	}
	void append(const T &t)					{ std::deque<T>::push_back(t);	}

	void removeAt(Int64 i)					{ std::deque<T>::erase( this->begin()+i );	}

	StdDeque &operator+=(const StdDeque<T> t)		{ return append(t);	}
	StdDeque &operator+=(const T &other)	{ append(other); return *this; 	}
	StdDeque &operator<<(const T &other)	{ append(other); return *this;	}

	Int64 count()const		{ return static_cast<int>(std::deque<T>::size());	}

	const T &at(Int64 i) const	{ return std::deque<T>::at(i);	}
	T &at(Int64 i)				{ return std::deque<T>::at(i);	}

	const T &first() const	{ return std::deque<T>::front();	}
	T &first()				{ return std::deque<T>::front();	}

	const T &last() const	{ return std::deque<T>::back();	}
	T &last()				{ return std::deque<T>::back();	}

	bool isEmpty()			{ return count() == 0;	}

	StdDeque filter( std::function<bool(const T&)> filterFnc ) const
	{
		StdDeque rtn;
		for( const T &t : *this )
			if( filterFnc(t) )
				rtn.append(t);
		return rtn;
	}
	Int64 indexOf( std::function<bool(const T&)> filterFnc, Int64 startsWith = 0 ) const
	{
		if( (startsWith >= 0) && (startsWith < count()) )
		{
			do
			{
				if( filterFnc(at(startsWith)) )
					return startsWith;
			}
			while( ++startsWith < count() );
		}
		return -1;
	}
};
}
#endif // STDDEQUE_H
