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
template <typename T>

class StdDeque : public std::deque<T>
{
public:
	StdDeque &append(const StdDeque<T> &t)
	{
		StdDeque<T>::push_back(t);
	}
	StdDeque &operator+=(const StdDeque<T> t)		{ return append(t);	}
	void append(const T &t)
	{
		StdDeque<T>::push_back(t);
	}
	StdDeque &operator+=(const T &other)	{ append(other); return *this; 	}
	StdDeque &operator<<(const T &other)	{ append(other); return *this;	}
	int count()const	{ return static_cast<int>(StdDeque<T>::size());	}

	const T &first() const	{ return StdDeque<T>::front();	}
	T &first()				{ return StdDeque<T>::front();	}

	const T &last() const	{ return StdDeque<T>::back();	}
	T &last()				{ return StdDeque<T>::back();	}

	bool isEmpty()			{ return count() == 0;	}
};

#endif // STDDEQUE_H