
// MainServer.h : MainServer Ӧ�ó������ͷ�ļ�
//
#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"       // ������


// CMainServerApp:
// �йش����ʵ�֣������ MainServer.cpp
//

class CMainServerApp : public CWinApp
{
public:
	CMainServerApp();


// ��д
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	virtual BOOL OnIdle(LONG lCount);

// ʵ��

public:
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()

protected:
	time_t		m_lastTime;
};

extern CMainServerApp theApp;
