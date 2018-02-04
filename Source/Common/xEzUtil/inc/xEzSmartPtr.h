/************************************************************************/
/* All rights reserved.
/* You can't use, copy or modify the file without the permission of the
/* author. Otherwise I will be held accountable under the law.
/*
/* Author: xiaoxin
/* Contact: 765392700(QQ)
/* Date: 2018/1/24
/************************************************************************/
#ifndef __XEZ_SMARTPTR_H__
#define __XEZ_SMARTPTR_H__

#include "xEzCompile.h"
#include "xEzMacro.h"
#include "xEzDebug.h"
#include "xEzMisc.h"

#ifdef EZ_SINGLE_THREAD
typedef int	EzRefCounter;
#else
class EZ_DLL_SPEC EzRefCounter
{
public:
	EzRefCounter()
		: m_nCount(0)
	{}

	EzRefCounter(int nCount)
		: m_nCount(nCount)
	{}

	EzRefCounter& operator= (int nCount);

	operator int() const;

	EzRefCounter& operator++ ();
	EzRefCounter operator++ (int);

	EzRefCounter& operator-- ();
	EzRefCounter operator-- (int);
	
private:
	volatile int m_nCount;
};
#endif


template<class T>
class EzSharedPtr
{
public:
	EzSharedPtr()
		: m_pObj(NULL)
		, m_pRefs(NULL)
	{}

	EzSharedPtr(T* pObj)
		: m_pObj(pObj)
		, m_pRefs(NULL)
	{
		if (m_pObj != NULL)
		{
			m_pRefs = new EzRefCounter(1);
			EzAssert(m_pRefs != NULL);
		}
	}

	EzSharedPtr(const EzSharedPtr& other)
		: m_pObj(other.m_pObj)
		, m_pRefs(other.m_pRefs)
	{
		if (m_pRefs != NULL)
		{
			EzAssert(m_pObj != NULL);
			++(*m_pRefs);
		}
	}

	~EzSharedPtr()
	{
		if (m_pRefs != NULL)
		{
			--(*m_pRefs);
			if (0 == *m_pRefs)
			{
				EzAssert(m_pObj != NULL);
				EZ_SAFE_DELETE(m_pObj);

				EZ_SAFE_DELETE(m_pRefs);
			}
		}
	}

	EzSharedPtr& operator= (const EzSharedPtr& other)
	{
		EzSharedPtr tmp(other);
		tmp.swap(*this);
		return *this;
	}

	EzSharedPtr& operator= (T* pObj)
	{
		EzSharedPtr tmp(pObj);
		tmp.swap(*this);
		return *this;
	}

	T& operator* ()
	{
		EzAssert(m_pObj != NULL);
		return *m_pObj;
	}

	T* operator-> ()
	{
		return m_pObj;
	}

	operator bool() const
	{
		return m_pObj != NULL;
	}

	T* get() const
	{
		return m_pObj;
	}

	EzRefCounter refCount() const
	{
		return (m_pRefs != NULL) ? *m_pRefs : 0;
	}

	void reset()
	{
		EzSharedPtr().swap(*this);
	}

	void swap(EzSharedPtr& other)
	{
		EzSwap(m_pObj, other.m_pObj);
		EzSwap(m_pRefs, other.m_pRefs);
	}

private:
	T*				m_pObj;
	EzRefCounter*	m_pRefs;
};

#endif // __XEZ_SMARTPTR_H__