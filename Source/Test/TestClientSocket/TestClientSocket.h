
// TestClientSocket.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CTestClientSocketApp: 
// �йش����ʵ�֣������ TestClientSocket.cpp
//

class CTestClientSocketApp : public CWinApp
{
public:
	CTestClientSocketApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CTestClientSocketApp theApp;