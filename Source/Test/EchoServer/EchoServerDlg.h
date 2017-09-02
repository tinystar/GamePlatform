
// EchoServerDlg.h : ͷ�ļ�
//

#pragma once

#include "EchoServerTest.h"
#include "IServerUIObserver.h"

#define WM_CLIENT_CONNECT		(WM_USER + 1)
#define WM_CLIENT_CLOSE			(WM_USER + 2)


// CEchoServerDlg �Ի���
class CEchoServerDlg : public CDialogEx, public IServerUIObserver
{
// ����
public:
	CEchoServerDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_ECHOSERVER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

protected:
	virtual void onClientConnect(ClientId id);
	virtual void onClientClose(ClientId id);

// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnDestroy();
	afx_msg LRESULT OnClientConnUIMsg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnClientCloseUIMsg(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

private:
	EchoServer m_echoServer;
	TimeServer m_timeServer;
	CListCtrl* m_pClientList;
};
