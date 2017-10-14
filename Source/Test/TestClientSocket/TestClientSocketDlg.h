
// TestClientSocketDlg.h : 头文件
//

#pragma once
#include "TcpClientSocket.h"
#include <fstream>

#define MAX_ECHOMSG_SIZE		4096


// CTestClientSocketDlg 对话框
class CTestClientSocketDlg : public CDialogEx, public ITcpClientSocketEventHandler
{
// 构造
public:
	CTestClientSocketDlg(CWnd* pParent = NULL);	// 标准构造函数
	~CTestClientSocketDlg();

// 对话框数据
	enum { IDD = IDD_TESTCLIENTSOCKET_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

	static unsigned __stdcall selectThread(void* pParam);

protected:
	virtual void onSocketConnected(TcpClientSocket* pClientSock);
	virtual void onSocketRecved(TcpClientSocket* pClientSock, void* pPackage, size_t nSize);
	virtual void onSocketClosed(TcpClientSocket* pClientSock, int nErrCode);

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnTest1Clicked();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	DECLARE_MESSAGE_MAP()

protected:
	CString			m_sSrcFile;
	CString			m_sDstFile;
	CEdit			m_editTest1;
	TcpClientSocket m_sockTest1;
	size_t			m_nFileSize;
	size_t			m_nRecvSize;
	std::ofstream	m_recvFile;
};
