
// TestClientDlg.h : ͷ�ļ�
//

#pragma once
#include "ClientSocket.h"


// CTestClientDlg �Ի���
class CTestClientDlg : public CDialogEx
{
// ����
public:
	CTestClientDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_TESTCLIENT_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
