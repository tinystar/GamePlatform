
// TestClientSocketDlg.h : ͷ�ļ�
//

#pragma once
#include "TcpClientSocket.h"
#include <fstream>

#define MAX_ECHOMSG_SIZE		4096


// CTestClientSocketDlg �Ի���
class CTestClientSocketDlg : public CDialogEx, public ITcpClientSocketEventHandler
{
// ����
public:
	CTestClientSocketDlg(CWnd* pParent = NULL);	// ��׼���캯��
	~CTestClientSocketDlg();

// �Ի�������
	enum { IDD = IDD_TESTCLIENTSOCKET_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

	static unsigned __stdcall selectThread(void* pParam);

protected:
	virtual void onSocketConnected(TcpClientSocket* pClientSock);
	virtual void onSocketRecved(TcpClientSocket* pClientSock, void* pPackage, size_t nSize);
	virtual void onSocketClosed(TcpClientSocket* pClientSock, int nErrCode);

// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
