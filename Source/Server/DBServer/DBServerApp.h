
// DBServer.h : DBServer Ӧ�ó������ͷ�ļ�
//
#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"       // ������


// CDBServerApp:
// �йش����ʵ�֣������ DBServer.cpp
//

class CDBServerApp : public CWinApp
{
public:
	CDBServerApp();


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

extern CDBServerApp theApp;
