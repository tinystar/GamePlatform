/************************************************************************/
/* All rights reserved.
/* You can't use, copy or modify the file without the permission of the
/* author. Otherwise I will be held accountable under the law.
/*
/* Author: xiaoxin
/* Contact: 765392700(QQ)
/* Date: 2017/1/7
/************************************************************************/
#ifndef __SERVICE_EXCP_H__
#define __SERVICE_EXCP_H__

#include "CoreService.h"

class ServiceExcp : public EzException
{
public:
	explicit ServiceExcp(TCHAR* pszExcpInfo, SVCErrorCode errCode, TCHAR* pszFile = NULL, int nLine = 0)
		: EzException(pszExcpInfo, pszFile, nLine)
		, m_errCode(errCode)
	{
	}

	SVCErrorCode errorCode() const { return m_errCode; }

private:
	SVCErrorCode	m_errCode;
};

#endif // __SERVICE_EXCP_H__
