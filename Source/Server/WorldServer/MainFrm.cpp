
// MainFrm.cpp : CMainFrame ���ʵ��
//

#include "stdafx.h"
#include "WorldServerApp.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define ID_TRAY_ICON			100


// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_MESSAGE(WM_TRAY_NOTIFY, &CMainFrame::OnTrayNotify)
	ON_COMMAND(ID_TRAYMENU_OPEN, &CMainFrame::OnTraymenuOpen)
	ON_COMMAND(ID_TRAYMENU_EXIT, &CMainFrame::OnTraymenuExit)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // ״̬��ָʾ��
	ID_INDICATOR_SVRINFO,
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame ����/����

CMainFrame::CMainFrame()
	: m_trayIcon(ID_TRAY_ICON)
	, m_bIconAdded(false)
{
	// TODO:  �ڴ���ӳ�Ա��ʼ������
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// ����һ����ͼ��ռ�ÿ�ܵĹ�����
	if (!m_wndView.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW, CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL))
	{
		TRACE0("δ�ܴ�����ͼ����\n");
		return -1;
	}

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("δ�ܴ���״̬��\n");
		return -1;      // δ�ܴ���
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));
	m_wndStatusBar.SetPaneText(1, NULL);

	m_trayIcon.Init(this->GetSafeHwnd(), WM_TRAY_NOTIFY);

	HICON hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	SetIcon(hIcon, FALSE);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO:  �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ
	cs.cx = 913;
	cs.cy = 635;

	cs.style = WS_OVERLAPPED | WS_CAPTION | FWS_ADDTOTITLE
		| WS_MINIMIZEBOX | WS_SYSMENU;

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);
	return TRUE;
}

// CMainFrame ���

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG


// CMainFrame ��Ϣ�������

void CMainFrame::OnSetFocus(CWnd* /*pOldWnd*/)
{
	// ������ǰ�Ƶ���ͼ����
	m_wndView.SetFocus();
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// ����ͼ��һ�γ��Ը�����
	if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	// ����ִ��Ĭ�ϴ���
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
	CFrameWnd::OnSize(nType, cx, cy);
	if (SIZE_MINIMIZED == nType && m_bIconAdded)
		ShowWindow(SW_HIDE);
}

void CMainFrame::OnDestroy()
{
	m_trayIcon.DeleteIcon();
	CFrameWnd::OnDestroy();
}

LRESULT CMainFrame::OnTrayNotify(WPARAM wParam, LPARAM lParam)
{
	switch (lParam)
	{
	case WM_RBUTTONUP:
		{
			CMenu menu;
			menu.LoadMenu(IDR_MENUTRAY);
			CMenu* pTrayMenu = menu.GetSubMenu(0);
			CPoint pt;
			GetCursorPos(&pt);
			SetForegroundWindow();
			pTrayMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, this);
			return 1;
		}
	case WM_LBUTTONDBLCLK:
		{
			SetForegroundWindow();
			ShowWindow(SW_SHOWNORMAL);
			return 1;
		}
	}

	return 0;
}

void CMainFrame::OnTraymenuOpen()
{
	SetForegroundWindow();
	ShowWindow(SW_SHOWNORMAL);
}

void CMainFrame::OnTraymenuExit()
{
	SendMessage(WM_CLOSE);
}
