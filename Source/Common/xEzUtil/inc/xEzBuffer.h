/************************************************************************/
/* All rights reserved.
/* You can't use, copy or modify the file without the permission of the
/* author. Otherwise I will be held accountable under the law.
/* 
/* Author: xiaoxin
/* Contact: 765392700(QQ)
/* Date: 2016/12/4
/************************************************************************/
#ifndef __XEZ_BUFFER_H__
#define __XEZ_BUFFER_H__

#include "xEzCompile.h"
#include <stddef.h>
#include <iosfwd>

template<typename T, size_t N>
class EzFixedBuf
{
public:
	EzFixedBuf();
	~EzFixedBuf();

public:
	size_t capacity() const { return N; }
	size_t size() const { return m_size; }
	const T* buffer() const { return m_pBuffer; }
	bool isEmpty() const { return m_size == 0; }

	/**
	 * return 0 if there is an error
	 * return -1 if there is not enough space
	 * otherwise return the number of elements to write successfully
	 */
	EZFORCEINLINE int write(const T* pElm, size_t count);

	EZFORCEINLINE bool flush(std::ostream& os);

	void reset() { m_size = 0; }

private:
	T			m_pBuffer[N];
	size_t		m_size;
};

template<typename T, size_t N>
EzFixedBuf<T, N>::EzFixedBuf()
	: m_size(0)
{
	::memset(m_pBuffer, 0, sizeof(m_pBuffer));
}

template<typename T, size_t N>
EzFixedBuf<T, N>::~EzFixedBuf()
{
}

template<typename T, size_t N>
int EzFixedBuf<T, N>::write(const T* pElm, size_t count)
{
	if (NULL == pElm)
		return 0;
	if (0 == count)
		return 0;

	if (m_size + count > N)
		return -1;

	::memcpy(m_pBuffer + m_size, pElm, count * sizeof(T));
	m_size += count;

	return count;
}

template<typename T, size_t N>
bool EzFixedBuf<T, N>::flush(std::ostream& os)
{
	os.write((const char*)m_pBuffer, m_size * sizeof(T));
	if (os)
	{
		reset();
		return true;
	}
	return false;
}

#endif // __XEZ_BUFFER_H__