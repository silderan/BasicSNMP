#ifndef STDLIST_H
#define STDLIST_H

#include <list>

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
	int count()const	{ return static_cast<int>(std::list<T>::size());	}

	const T &first() const	{ return std::list<T>::front();	}
	T &first()				{ return std::list<T>::front();	}

	const T &last() const	{ return std::list<T>::back();	}
	T &last()				{ return std::list<T>::back();	}

	bool isEmpty()			{ return count() == 0;	}
};

#endif // STDLIST_H
