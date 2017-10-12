/************************************************************************/
/* All rights reserved.
/* You can't use, copy or modify the file without the permission of the
/* author. Otherwise I will be held accountable under the law.
/*
/* Author: xiaoxin
/* Contact: 765392700(QQ)
/* Date: 2017/2/19
/************************************************************************/
#ifndef __XEZ_ARRAY_H__
#define __XEZ_ARRAY_H__

#include "xEzDebug.h"
#include "xEzMacro.h"
#ifdef _MSC_VER
#include <type_traits>
#elif __GNUC__
#include <tr1/type_traits>
#else
#error Need to specify path of type_traits!
#endif

template<typename T>
class EzArrayMemCopyReallocator
{
public:
	static void reallocateArray(T* pCopy, const T* pSource, int nCount)
	{
		EzAssert(nCount >= 0);
		if (nCount > 0)
		{
			memcpy(pCopy, pSource, nCount * sizeof(T));
		}
	}
};

template<typename T>
class EzArrayObjectCopyReallocator
{
public:
	static void reallocateArray(T* pCopy, const T* pSource, int nCount)
	{
		EzAssert(nCount >= 0);
		while (nCount--)
		{
			*pCopy++ = *pSource++;
		}
	}
};

template<typename T, bool>
struct AllocatorSelector;

template<typename T>
struct AllocatorSelector < T, false >
{
	typedef EzArrayObjectCopyReallocator<T> allocator;
};

template<typename T>
struct AllocatorSelector < T, true >
{
	typedef EzArrayMemCopyReallocator<T> allocator;
};


template<typename T, typename R = typename AllocatorSelector<T, std::tr1::is_pod<T>::value>::allocator >
class EzArray
{
public:
	EzArray(int initPhysicalLength = 0, int initGrowLength = 8);
	EzArray(const EzArray<T, R>&);
	~EzArray();

	EzArray<T, R>& operator= (const EzArray<T, R>&);

	bool operator== (const EzArray<T, R>&) const;

	T& operator[] (int);
	const T& operator[] (int) const;

	const T& at(int index) const;
	T& at(int index);

	EzArray<T, R>& setAt(int index, const T& value);
	EzArray<T, R>& setAll(const T& value);

	T& first();
	const T& first() const;
	T& last();
	const T& last() const;

	int append(const T& value);
	EzArray<T, R>& append(const EzArray<T, R>& array);
	EzArray<T, R>& insertAt(int index, const T& value);

	EzArray<T, R>& removeAt(int index);
	bool remove(const T& value, int start = 0);
	EzArray<T, R>& removeFirst();
	EzArray<T, R>& removeLast();
	EzArray<T, R>& removeAll();
	EzArray<T, R>& removeSubArray(int startIndex, int endIndex);

	bool contains(const T& value, int start = 0) const;
	bool find(const T& value, int& foundAt, int start = 0) const;
	int find(const T& value) const;
	int findFrom(const T& value, int start) const;

	int length() const;
	bool isEmpty() const;
	int logicalLength() const;
	EzArray<T, R>& setLogicalLength(int);
	int physicalLength() const;
	EzArray<T, R>& setPhysicalLength(int);

	int growLength() const;
	EzArray<T, R>& setGrowLength(int);

	EzArray<T, R>& reverse();
	EzArray<T, R>& swap(int i1, int i2);

	const T* asArrayPtr() const;
	T* asArrayPtr();

protected:
	T*		mpArray;
	int		mPhysicalLen;
	int		mLogicalLen;
	int		mGrowLen;

	bool isValid(int) const;
};

template<typename T, typename R>
EzArray<T, R>::EzArray(int initPhysicalLength /*= 0*/, int initGrowLength /*= 8*/)
	: mpArray(NULL)
	, mPhysicalLen(initPhysicalLength)
	, mLogicalLen(0)
	, mGrowLen(initGrowLength)
{
	EzAssert(mPhysicalLen >= 0);
	EzAssert(mGrowLen > 0);
	if (mPhysicalLen > 0)
	{
		mpArray = new T[mPhysicalLen];

		if (NULL == mpArray)
		{
			mPhysicalLen = 0;
			EzAssert(false);
		}
	}
}

template<typename T, typename R>
EzArray<T, R>::EzArray(const EzArray<T, R>& src)
	: mpArray(NULL)
	, mPhysicalLen(src.mPhysicalLen)
	, mLogicalLen(src.mLogicalLen)
	, mGrowLen(src.mGrowLen)
{
	if (mPhysicalLen > 0)
	{
		mpArray = new T[mPhysicalLen];

		if (NULL == mpArray)
		{
			mPhysicalLen = 0;
			mLogicalLen = 0;
			EzAssert(false);
		}
	}

	R::reallocateArray(mpArray, src.mpArray, mLogicalLen);
}

template<typename T, typename R>
EzArray<T, R>::~EzArray()
{
	if (mpArray != NULL)
		delete[] mpArray;
}

template<typename T, typename R>
EzArray<T, R>& EzArray<T, R>::operator=(const EzArray<T, R>& src)
{
	if (this != &src)
	{
		if (mPhysicalLen < src.mLogicalLen)
		{
			if (mpArray != NULL)
				delete[] mpArray;
			mPhysicalLen = src.mLogicalLen;
			mpArray = new T[mPhysicalLen];

			if (NULL == mpArray)
			{
				mPhysicalLen = 0;
				mLogicalLen = 0;
				EzAssert(false);
				return *this;
			}
		}
		mLogicalLen = src.mLogicalLen;
		R::reallocateArray(mpArray, src.mpArray, mLogicalLen);
	}
	return *this;
}

template<typename T, typename R>
bool EzArray<T, R>::operator==(const EzArray<T, R>& cpr) const
{
	if (mLogicalLen == cpr.mLogicalLen)
	{
		for (int i = 0; i < mLogicalLen; i++)
			if (mpArray[i] != cpr.mpArray[i])
				return false;
		return true;
	}
	return false;
}

template<typename T, typename R>
T& EzArray<T, R>::operator[](int i)
{
	EzAssert(isValid(i));
	return mpArray[i];
}

template<typename T, typename R>
const T& EzArray<T, R>::operator[](int i) const
{
	EzAssert(isValid(i));
	return mpArray[i];
}

template<typename T, typename R>
const T& EzArray<T, R>::at(int index) const
{
	EzAssert(isValid(index));
	return mpArray[index];
}

template<typename T, typename R>
T& EzArray<T, R>::at(int index)
{
	EzAssert(isValid(index));
	return mpArray[index];
}

template<typename T, typename R>
EzArray<T, R>& EzArray<T, R>::setAt(int index, const T& value)
{
	EzAssert(isValid(index));
	mpArray[index] = value;
	return *this;
}

template<typename T, typename R>
EzArray<T, R>& EzArray<T, R>::setAll(const T& value)
{
	for (int i = 0; i < mLogicalLen; i++)
	{
		mpArray[i] = value;
	}
	return *this;
}

template<typename T, typename R>
T& EzArray<T, R>::first()
{
	EzAssert(!isEmpty());
	return mpArray[0];
}

template<typename T, typename R>
const T& EzArray<T, R>::first() const
{
	EzAssert(!isEmpty());
	return mpArray[0];
}

template<typename T, typename R>
T& EzArray<T, R>::last()
{
	EzAssert(!isEmpty());
	return mpArray[mLogicalLen - 1];
}

template<typename T, typename R>
const T& EzArray<T, R>::last() const
{
	EzAssert(!isEmpty());
	return mpArray[mLogicalLen - 1];
}

template<typename T, typename R>
int EzArray<T, R>::append(const T& value)
{
	insertAt(mLogicalLen, value);
	return mLogicalLen - 1;
}

template<typename T, typename R>
EzArray<T, R>& EzArray<T, R>::append(const EzArray<T, R>& array)
{
	int otherLen = array.length();
	if (0 == otherLen)
		return *this;

	int newLen = mLogicalLen + otherLen;
	if (newLen > mPhysicalLen)
		setPhysicalLength(mLogicalLen + EZMAX(otherLen, mGrowLen));

	R::reallocateArray(mpArray + mLogicalLen, array.mpArray, otherLen);

	mLogicalLen = newLen;
	return *this;
}

template<typename T, typename R>
EzArray<T, R>& EzArray<T, R>::insertAt(int index, const T& value)
{
	T tmp(value);
	EzAssert(0 <= index && index <= mLogicalLen);

	if (mLogicalLen >= mPhysicalLen)
		setPhysicalLength(mLogicalLen + mGrowLen);

	if (index != mLogicalLen)
	{
		T* p = mpArray + mLogicalLen;
		T* pStop = mpArray + index;
		do 
		{
			*p = *(p - 1);
		} while (--p != pStop);
	}
	mpArray[index] = tmp;
	mLogicalLen++;
	return *this;
}

template<typename T, typename R>
EzArray<T, R>& EzArray<T, R>::removeAt(int index)
{
	EzAssert(isValid(index));

	if (index < mLogicalLen - 1)
	{
		T* p = mpArray + index;
		T* pStop = mpArray + mLogicalLen - 1;
		do 
		{
			*p = *(p + 1);
		} while (++p != pStop);
	}

	EzAssert(!isEmpty());
	if (!isEmpty())
		mLogicalLen--;
	return *this;
}

template<typename T, typename R>
bool EzArray<T, R>::remove(const T& value, int start /*= 0*/)
{
	const int i = findFrom(value, start);
	if (i == -1)
		return false;

	removeAt(i);
	return true;
}

template<typename T, typename R>
EzArray<T, R>& EzArray<T, R>::removeFirst()
{
	EzAssert(!isEmpty());
	return removeAt(0);
}

template<typename T, typename R>
EzArray<T, R>& EzArray<T, R>::removeLast()
{
	EzAssert(!isEmpty());
	if (!isEmpty())
		mLogicalLen--;
	return *this;
}

template<typename T, typename R>
EzArray<T, R>& EzArray<T, R>::removeAll()
{
	setLogicalLength(0);
	return *this;
}

template<typename T, typename R>
EzArray<T, R>& EzArray<T, R>::removeSubArray(int startIndex, int endIndex)
{
	EzAssert(isValid(startIndex));
	EzAssert(startIndex <= endIndex);

	if (endIndex >= mLogicalLen - 1)
	{
		mLogicalLen = startIndex;
		return *this;
	}

	if (startIndex < mLogicalLen - 1)
	{
		T* p = mpArray + startIndex;
		T* q = mpArray + endIndex + 1;
		T* pStop = mpArray + mLogicalLen;
		for (; q < pStop; p++, q++)
			*p = *q;
	}
	mLogicalLen -= endIndex - startIndex + 1;
	return *this;
}

template<typename T, typename R>
bool EzArray<T, R>::contains(const T& value, int start /*= 0*/) const
{
	return findFrom(value, start) != -1;
}

template<typename T, typename R>
bool EzArray<T, R>::find(const T& value, int& foundAt, int start /*= 0*/) const
{
	const int nFoundAt = findFrom(value, start);
	if (-1 == nFoundAt)
		return false;

	foundAt = nFoundAt;
	return true;
}

template<typename T, typename R>
int EzArray<T, R>::find(const T& value) const
{
	return findFrom(value, 0);
}

template<typename T, typename R>
int EzArray<T, R>::findFrom(const T& value, int start) const
{
	for (int i = start; i < mLogicalLen; i++)
		if (mpArray[i] == value)
			return i;

	return -1;
}

template<typename T, typename R>
int EzArray<T, R>::length() const
{
	return mLogicalLen;
}

template<typename T, typename R>
bool EzArray<T, R>::isEmpty() const
{
	return mLogicalLen == 0;
}

template<typename T, typename R>
int EzArray<T, R>::logicalLength() const
{
	return mLogicalLen;
}

template<typename T, typename R>
EzArray<T, R>& EzArray<T, R>::setLogicalLength(int len)
{
	EzAssert(len >= 0);
	if (len > mPhysicalLen)
	{
		int growth = EZMAX(len - mPhysicalLen, mGrowLen);
		setPhysicalLength(mPhysicalLen + growth);
	}

	mLogicalLen = (len >= 0) ? len : 0;
	return *this;
}

template<typename T, typename R>
int EzArray<T, R>::physicalLength() const
{
	return mPhysicalLen;
}

template<typename T, typename R>
EzArray<T, R>& EzArray<T, R>::setPhysicalLength(int len)
{
	EzAssert(len >= 0);
	if (len == mPhysicalLen)
		return *this;

	T* pOldArray = mpArray;
	if (0 == len)
	{
		mpArray = NULL;
		mPhysicalLen = 0;
	}
	else
	{
		mpArray = new T[len];
		if (NULL == mpArray)
		{
			mPhysicalLen = 0;
			EzAssert(false);
		}
		else
		{
			R::reallocateArray(mpArray, pOldArray, ((len < mLogicalLen) ? len : mLogicalLen));
			mPhysicalLen = len;
		}
	}

	if (pOldArray != NULL)
		delete[] pOldArray;
	if (mPhysicalLen < mLogicalLen)
		mLogicalLen = mPhysicalLen;
	return *this;
}

template<typename T, typename R>
int EzArray<T, R>::growLength() const
{
	return mGrowLen;
}

template<typename T, typename R>
EzArray<T, R>& EzArray<T, R>::setGrowLength(int len)
{
	EzAssert(len > 0);
	mGrowLen = len;
	return *this;
}

template<typename T, typename R>
EzArray<T, R>& EzArray<T, R>::reverse()
{
	for (int i = 0; i < mLogicalLen / 2; i++)
	{
		T tmp = mpArray[i];
		mpArray[i] = mpArray[mLogicalLen - 1 - i];
		mpArray[mLogicalLen - 1 - i] = tmp;
	}
	return *this;
}

template<typename T, typename R>
EzArray<T, R>& EzArray<T, R>::swap(int i1, int i2)
{
	EzAssert(isValid(i1));
	EzAssert(isValid(i2));

	if (i1 == i2)
		return *this;

	T tmp = mpArray[i1];
	mpArray[i1] = mpArray[i2];
	mpArray[i2] = tmp;
	return *this;
}

template<typename T, typename R>
const T* EzArray<T, R>::asArrayPtr() const
{
	return mpArray;
}

template<typename T, typename R>
T* EzArray<T, R>::asArrayPtr()
{
	return mpArray;
}

template<typename T, typename R>
bool EzArray<T, R>::isValid(int idx) const
{
	return idx >= 0 && idx < mLogicalLen;
}

#endif // __XEZ_ARRAY_H__