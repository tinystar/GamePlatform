/************************************************************************/
/* All rights reserved.
/* You can't use, copy or modify the file without the permission of the
/* author. Otherwise I will be held accountable under the law.
/*
/* Author: xiaoxin
/* Contact: 765392700(QQ)
/* Date: 2016/12/8
/************************************************************************/
#ifndef __XEZ_EXCEPTION_H__
#define __XEZ_EXCEPTION_H__

#include "xEzMacro.h"
#include <tchar.h>

class EzException
{
public:
	explicit EzException(TCHAR* pszExcpInfo, TCHAR* pszFile = NULL, int nLine = 0)
		: m_pszFile(pszFile)
		, m_nLineNumb(nLine)
	{
		copyStr(pszExcpInfo);
	}
	virtual ~EzException()
	{
		EZ_SAFE_DELETE_ARRAY(m_pszExcpInfo);
	}
	EzException(const EzException& other)
		: m_pszExcpInfo(NULL)
		, m_pszFile(other.m_pszFile)
		, m_nLineNumb(other.m_nLineNumb)
	{
		copyStr(other.m_pszExcpInfo);
	}
	EzException& operator= (const EzException& other)
	{
		if (this != &other)
		{
			EZ_SAFE_DELETE_ARRAY(m_pszExcpInfo);

			copyStr(other.m_pszExcpInfo);
			m_pszFile = other.m_pszFile;
			m_nLineNumb = other.m_nLineNumb;
		}

		return *this;
	}

	virtual const TCHAR* info() const { return m_pszExcpInfo ? m_pszExcpInfo : _T("unknown info"); }
	const TCHAR* file() const { return m_pszFile ? m_pszFile : _T("null"); }
	int line() const { return m_nLineNumb; }

private:
	void copyStr(const TCHAR* pszInfo)
	{
		int nLen = _tcslen(pszInfo);
		m_pszExcpInfo = new TCHAR[nLen + 1];
		_tcscpy_s(m_pszExcpInfo, nLen + 1, pszInfo);
	}

protected:
	TCHAR*				m_pszExcpInfo;
	const TCHAR*		m_pszFile;
	int					m_nLineNumb;
};

#endif // __XEZ_EXCEPTION_H__