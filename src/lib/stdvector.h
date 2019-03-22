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
