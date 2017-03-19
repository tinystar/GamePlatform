/************************************************************************/
/* All rights reserved.
/* You can't use, copy or modify the file without the permission of the
/* author. Otherwise I will be held accountable under the law.
/*
/* Author: xiaoxin
/* Contact: 765392700(QQ)
/* Date: 2016/11/27
/*		 2017/1/19 cross-platform modified.
/************************************************************************/
#ifndef __XEZ_LOCK_H__
#define __XEZ_LOCK_H__

#include "xEzCompile.h"

class EzLockImp;
class EzRWLockImp;

class EZ_DLL_SPEC EzLock
{
public:
	EzLock();
	~EzLock();

	void lock();
	void unlock();

private:
	EzLockImp*	m_pImpLock;
};

class EZ_DLL_SPEC EzAutoLocker
{
public:
	EzAutoLocker(EzLock* pLock)
		: m_pLock(pLock)
	{
		if (m_pLock)
			m_pLock->lock();
	}

	~EzAutoLocker()
	{
		if (m_pLock)
			m_pLock->unlock();
	}

private:
	EzLock* m_pLock;
};

class EZ_DLL_SPEC EzRWLock
{
public:
	EzRWLock();
	~EzRWLock();

	void lockRead();
	void lockWrite();

	void unlockRead();
	void unlockWrite();

private:
	EzRWLockImp* m_pImpLock;
};

#endif // __XEZ_LOCK_H__