
// TestServiceDlg.h : 头文件
//

#pragma once
#include "TestServer.h"
#include "TestTimer.h"


// CTestServiceDlg 对话框
class CTestServiceDlg : public CDialogEx
{
// 构造
public:
	CTestServiceDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_TESTSERVICE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnDestroy();
	afx_msg void OnClickedButtonTest();
	afx_msg void OnClickedButtonTimer();
	DECLARE_MESSAGE_MAP()

protected:
	TestServer m_server;
	TestTimer  m_timer;
};
