
// EchoServerDlg.h : ͷ�ļ�
//

#pragma once

#include "EchoServerTest.h"


// CEchoServerDlg �Ի���
class CEchoServerDlg : public CDialogEx
{
// ����
public:
	CEchoServerDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_ECHOSERVER_DIALOG };

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
	DECLARE_MESSAGE_MAP()

private:
	EchoServer m_echoServer;
	TimeServer m_timeServer;
};
