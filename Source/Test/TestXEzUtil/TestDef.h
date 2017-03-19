/************************************************************************/
/* Author: xiaoxin
/* Contact: 765392700(QQ)
/* Date: 2017/1/11
/************************************************************************/
#ifndef __TEST_DEF_H__
#define __TEST_DEF_H__

#include "xEzUtil.h"

class TestNC		// none copyable
{
EZ_FORBID_CLASS_COPY(TestNC)

public:
	TestNC()
		: m_nNum(0)
		, m_pTestStr(NULL)
	{}

	TestNC(int num, const TCHAR* pszStr)
		: m_nNum(num)
		, m_pTestStr(pszStr)
	{}

	void setNum(int num) { m_nNum = num; }
	void setString(const TCHAR* pszStr) { m_pTestStr = pszStr; }

	void print()
	{
		EzTrace(_T("TestNC num = %d, test string = %s\n"), m_nNum, m_pTestStr);
	}

private:
	int m_nNum;
	const TCHAR* m_pTestStr;
};

#endif // __TEST_DEF_H__