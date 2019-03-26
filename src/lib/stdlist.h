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

#ifndef STDLIST_H
#define STDLIST_H

#include <list>

#include "basic_types.h"

// This class doesn't have at() or operator[]() because it's a
// linked list and such functions will be costly. Use StdDeque if need such functions.
namespace SNMP {

template <typename T>
class StdList : public std::list<T>
{
public:
	StdList &append(const StdList<T> &t)
	{
		std::list<T>::insert( std::end(*this), std::begin(t), std::end(t) );
	}
	StdList &operator+=(const StdList<T> t)		{ return append(t);	}
	void append(const T &t)
	{
		this->push_back(t);
	}
	StdList &operator+=(const T &other)	{ append(other); return *this; 	}
	StdList &operator<<(const T &other)	{ append(other); return *this;	}
	Int64 count()const	{ return static_cast<Int64>(std::list<T>::size());	}

	const T &first() const	{ return std::list<T>::front();	}
	T &first()				{ return std::list<T>::front();	}

	const T &last() const	{ return std::list<T>::back();	}
	T &last()				{ return std::list<T>::back();	}

	bool isEmpty()			{ return count() == 0;	}
};

}
#endif // STDLIST_H
