#include "xEzSmartPtr.h"
#include <windows.h>


EzRefCounter& EzRefCounter::operator=(int nCount)
{
	InterlockedExchange((LONG*)&m_nCount, nCount);
	return *this;
}

EzRefCounter::operator int() const
{
	return InterlockedExchangeAdd((LONG*)&m_nCount, 0);
}

EzRefCounter& EzRefCounter::operator++()
{
	InterlockedExchangeAdd((LONG*)&m_nCount, 1);
	return *this;
}

EzRefCounter EzRefCounter::operator++(int)
{
	EzRefCounter tmp(m_nCount);
	++(*this);
	return tmp;
}

EzRefCounter& EzRefCounter::operator--()
{
	InterlockedExchangeAdd((LONG*)&m_nCount, -1);
	return *this;
}

EzRefCounter EzRefCounter::operator--(int)
{
	EzRefCounter tmp(m_nCount);
	--(*this);
	return tmp;
}
