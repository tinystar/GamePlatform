
// TimerTest.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CTimerTestApp: 
// �йش����ʵ�֣������ TimerTest.cpp
//

class CTimerTestApp : public CWinApp
{
public:
	CTimerTestApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CTimerTestApp theApp;