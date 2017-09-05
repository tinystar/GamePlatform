
// DBServer.h : DBServer 应用程序的主头文件
//
#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"       // 主符号


// CDBServerApp:
// 有关此类的实现，请参阅 DBServer.cpp
//

class CDBServerApp : public CWinApp
{
public:
	CDBServerApp();


// 重写
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	virtual BOOL OnIdle(LONG lCount);

// 实现

public:
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()

protected:
	time_t		m_lastTime;
};

extern CDBServerApp theApp;
