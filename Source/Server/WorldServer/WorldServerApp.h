
// WorldServer.h : WorldServer Ӧ�ó������ͷ�ļ�
//
#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"       // ������


// CWorldServerApp:
// �йش����ʵ�֣������ WorldServer.cpp
//

class CWorldServerApp : public CWinApp
{
public:
	CWorldServerApp();


// ��д
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// ʵ��

public:
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CWorldServerApp theApp;
