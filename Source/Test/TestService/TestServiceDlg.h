
// TestServiceDlg.h : ͷ�ļ�
//

#pragma once
#include "TestServer.h"


// CTestServiceDlg �Ի���
class CTestServiceDlg : public CDialogEx
{
// ����
public:
	CTestServiceDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_TESTSERVICE_DIALOG };

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
	afx_msg void OnClickedButtonTest();
	DECLARE_MESSAGE_MAP()

protected:
	TestServer m_server;
};
