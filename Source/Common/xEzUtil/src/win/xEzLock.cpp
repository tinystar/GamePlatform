#include <windows.h>
#include "xEzLock.h"
#include "xEzDebug.h"
#include "xEzMacro.h"


class EzLockImp
{
public:
	EzLockImp() { ::InitializeCriticalSection(&m_cs); }
	~EzLockImp() { ::DeleteCriticalSection(&m_cs); }

	void lock() { ::EnterCriticalSection(&m_cs); }
	void unlock() { ::LeaveCriticalSection(&m_cs); }

private:
	CRITICAL_SECTION	m_cs;
};


//------------------------------------------------------
// EzLock
//------------------------------------------------------
EzLock::EzLock()
{
	m_pImpLock = new EzLockImp;
	EzAssert(m_pImpLock);
}

EzLock::~EzLock()
{
	EZ_SAFE_DELETE(m_pImpLock);
}

void EzLock::lock() const
{
	if (m_pImpLock)
		m_pImpLock->lock();
}

void EzLock::unlock() const
{
	if (m_pImpLock)
		m_pImpLock->unlock();
}


//------------------------------------------------------
// EzRWLockImp
//------------------------------------------------------
class EzRWLockImp
{
public:
	EzRWLockImp();
	~EzRWLockImp();

	void lockRead();
	void lockWrite();

	void unlockRead() { done(); }
	void unlockWrite() { done(); }

	void done();

private:
	CRITICAL_SECTION	m_cs;
	HANDLE				m_hSemReaders;
	HANDLE				m_hSemWriters;
	unsigned int		m_uWaitReaders;
	unsigned int		m_uWaitWriters;
	int					m_nActive;
};

EzRWLockImp::EzRWLockImp()
{
	m_nActive = m_uWaitReaders = m_uWaitWriters = 0;
	::InitializeCriticalSection(&m_cs);
	m_hSemReaders = ::CreateSemaphore(NULL, 0, MAXLONG, NULL);
	m_hSemWriters = ::CreateSemaphore(NULL, 0, MAXLONG, NULL);
}

EzRWLockImp::~EzRWLockImp()
{
	EzAssert(m_nActive == 0);
	::CloseHandle(m_hSemWriters);
	::CloseHandle(m_hSemReaders);
	::DeleteCriticalSection(&m_cs);
}

void EzRWLockImp::lockRead()
{
	::EnterCriticalSection(&m_cs);
	bool bWait = m_nActive < 0 || m_uWaitWriters > 0;
	if (bWait)
		m_uWaitReaders++;
	else
		m_nActive++;
	::LeaveCriticalSection(&m_cs);

	if (bWait)
		::WaitForSingleObject(m_hSemReaders, INFINITE);
}

void EzRWLockImp::lockWrite()
{
	::EnterCriticalSection(&m_cs);
	bool bWait = m_nActive != 0;
	if (bWait)
		m_uWaitWriters++;
	else
		m_nActive = -1;
	::LeaveCriticalSection(&m_cs);

	if (bWait)
		::WaitForSingleObject(m_hSemWriters, INFINITE);
}

void EzRWLockImp::done()
{
	::EnterCriticalSection(&m_cs);
	if (m_nActive > 0)
		m_nActive--;
	else
		m_nActive++;

	HANDLE hSem = NULL;
	LONG lCount = 1;

	if (m_nActive == 0)
	{
		if (m_uWaitWriters > 0)
		{
			m_nActive = -1;
			m_uWaitWriters--;
			hSem = m_hSemWriters;
		}
		else if (m_uWaitReaders > 0)
		{
			m_nActive = m_uWaitReaders;
			m_uWaitReaders = 0;
			hSem = m_hSemReaders;
			lCount = m_nActive;
		}
	}
	::LeaveCriticalSection(&m_cs);

	if (hSem != NULL)
		::ReleaseSemaphore(hSem, lCount, NULL);
}


//------------------------------------------------------
// EzRWLock
//------------------------------------------------------
EzRWLock::EzRWLock()
{
	m_pImpLock = new EzRWLockImp();
	EzAssert(m_pImpLock);
}

EzRWLock::~EzRWLock()
{
	EZ_SAFE_DELETE(m_pImpLock);
}

void EzRWLock::lockRead() const
{
	if (m_pImpLock)
		m_pImpLock->lockRead();
}

void EzRWLock::lockWrite() const
{
	if (m_pImpLock)
		m_pImpLock->lockWrite();
}

void EzRWLock::unlockRead() const
{
	if (m_pImpLock)
		m_pImpLock->unlockRead();
}

void EzRWLock::unlockWrite() const
{
	if (m_pImpLock)
		m_pImpLock->unlockWrite();
}
