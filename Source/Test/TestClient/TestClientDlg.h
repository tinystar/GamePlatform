
// TestClientDlg.h : 头文件
//

#pragma once
#include "ClientSocket.h"


// CTestClientDlg 对话框
class CTestClientDlg : public CDialogEx
{
// 构造
public:
	CTestClientDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_TESTCLIENT_DIALOG };

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
	afx_msg void OnSendClicked();
	afx_msg void OnTestClicked();
	afx_msg void OnUploadClicked();
	afx_msg void OnDownloadClicked();
	DECLARE_MESSAGE_MAP()

protected:
	ClientSocket	m_testClient;
	ClientSocket	m_uploadSock;
	ClientSocket	m_downloadSock;
};
