
// TestXEzUtil.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CTestXEzUtilApp: 
// �йش����ʵ�֣������ TestXEzUtil.cpp
//

class CTestXEzUtilApp : public CWinApp
{
public:
	CTestXEzUtilApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CTestXEzUtilApp theApp;