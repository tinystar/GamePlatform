
// EchoServerDlg.h : 头文件
//

#pragma once

#include "EchoServerTest.h"


// CEchoServerDlg 对话框
class CEchoServerDlg : public CDialogEx
{
// 构造
public:
	CEchoServerDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_ECHOSERVER_DIALOG };

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
	DECLARE_MESSAGE_MAP()

private:
	EchoServer m_echoServer;
	TimeServer m_timeServer;
};
