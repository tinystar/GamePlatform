
// EchoServer.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CEchoServerApp: 
// �йش����ʵ�֣������ EchoServer.cpp
//

class CEchoServerApp : public CWinApp
{
public:
	CEchoServerApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CEchoServerApp theApp;