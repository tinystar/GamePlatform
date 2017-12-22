
// ChildView.cpp : CChildView 类的实现
//

#include "stdafx.h"
#include "DBServerApp.h"
#include "ChildView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define ID_BTN_START			1000
#define ID_BTN_DUMP				1001
#define ID_BTN_UPDCFG			1002
#define ID_BTN_OPENDIR			1003

#define IDT_FIRST_STARTSVR		110


// UI notify msg
#define WM_DATABASE_OPEN		(WM_USER + 1)
#define WM_DATABASE_CLOSE		(WM_USER + 2)


// CChildView

CChildView::CChildView()
	: m_svrStatus(0)
	, m_bFirstStart(TRUE)
{
	m_dbSvrMgr.getServer().registerUIObserver(this);
}

CChildView::~CChildView()
{
}


BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_BN_CLICKED(ID_BTN_START, &CChildView::OnBtnStartClick)
	ON_BN_CLICKED(ID_BTN_DUMP, &CChildView::OnBtnDumpClick)
	ON_BN_CLICKED(ID_BTN_UPDCFG, &CChildView::OnBtnUpdCfgClick)
	ON_BN_CLICKED(ID_BTN_OPENDIR, &CChildView::OnBtnOpenDirClick)
	ON_MESSAGE(WM_DATABASE_OPEN, &CChildView::OnDatabaseOpenUIMsg)
	ON_MESSAGE(WM_DATABASE_CLOSE, &CChildView::OnDatabaseCloseUIMsg)
END_MESSAGE_MAP()



// CChildView 消息处理程序

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	//cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), reinterpret_cast<HBRUSH>(CTLCOLOR_DLG + 1), NULL);

	return TRUE;
}

void CChildView::OnPaint() 
{
	CPaintDC dc(this); // 用于绘制的设备上下文
	
	// TODO:  在此处添加消息处理程序代码
	
	// 不要为绘制消息而调用 CWnd::OnPaint()
}

int CChildView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_font.CreatePointFont(90, _T("MS Shell Dlg"));

	CString strRes;
	strRes.LoadString(IDS_STARTSERVER);

	CRect rcControl = { 10, 390, 110, 415 };
	m_startBtn.Create(strRes, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, rcControl, this, ID_BTN_START);
	m_startBtn.SetFont(&m_font);

	rcControl.OffsetRect(110, 0);
	m_dumpBtn.Create(_T("Dump"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, rcControl, this, ID_BTN_DUMP);
	m_dumpBtn.SetFont(&m_font);
	m_dumpBtn.EnableWindow(FALSE);

	rcControl.OffsetRect(110, 0);
	strRes.LoadString(IDS_UPDATECONFIG);
	m_updCfgBtn.Create(strRes, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, rcControl, this, ID_BTN_UPDCFG);
	m_updCfgBtn.SetFont(&m_font);
	m_updCfgBtn.EnableWindow(FALSE);

	rcControl.OffsetRect(110, 0);
	strRes.LoadString(IDS_OPENDIR);
	m_openDirBtn.Create(strRes, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, rcControl, this, ID_BTN_OPENDIR);
	m_openDirBtn.SetFont(&m_font);

	rcControl.SetRect(10, 10, 575, 280);
	m_msgEdit.Create(WS_CHILD | WS_VISIBLE | WS_BORDER | ES_WANTRETURN | ES_MULTILINE | ES_READONLY, rcControl, this, -1);

	SVCErrorCode ec = eOk;
	if ((ec = m_dbSvrMgr.initServer()) != eOk)
	{
		TCHAR szLog[512] = { 0 };
		_stprintf_s(szLog, 511, _T("server init failed, error code = %d\n"), ec);
		MessageBox(szLog);
		m_startBtn.EnableWindow(FALSE);
	}
	else
	{
		m_svrStatus |= kInited;
		OnBtnStartClick();
	}

	return 0;
}

void CChildView::OnBtnStartClick()
{
	SVCErrorCode ec = eOk;
	if (m_svrStatus & kStarted)
	{
		if ((ec = m_dbSvrMgr.stopServer()) != eOk)
		{
			TCHAR szLog[512] = { 0 };
			_stprintf_s(szLog, 511, _T("server stop failed, error code = %d\n"), ec);
			MessageBox(szLog);
		}
		else
		{
			m_svrStatus &= ~kStarted;
			CString strStart;
			strStart.LoadString(IDS_STARTSERVER);
			m_startBtn.SetWindowText(strStart);
			m_dumpBtn.EnableWindow(FALSE);
			m_updCfgBtn.EnableWindow(FALSE);
		}
	}
	else
	{
		if ((ec = m_dbSvrMgr.startServer()) != eOk)
		{
			TCHAR szLog[512] = { 0 };
			_stprintf_s(szLog, 511, _T("server start failed, error code = %d\n"), ec);
			MessageBox(szLog);
		}
		else
		{
			m_svrStatus |= kStarted;
			CString strStop;
			strStop.LoadString(IDS_STOPSERVER);
			m_startBtn.SetWindowText(strStop);
			m_dumpBtn.EnableWindow(TRUE);
			m_updCfgBtn.EnableWindow(TRUE);

			if (m_bFirstStart)
			{
				SetTimer(IDT_FIRST_STARTSVR, 1000, NULL);
				m_bFirstStart = FALSE;
			}
		}
	}
}

void CChildView::OnBtnDumpClick()
{

}

void CChildView::OnBtnUpdCfgClick()
{

}

void CChildView::OnBtnOpenDirClick()
{
	TCHAR szPath[MAX_PATH] = { 0 };
	bool bRet = EzSys::getCurModulePath(szPath, MAX_PATH);
	if (bRet)
		::ShellExecute(NULL, _T("open"), szPath, NULL, NULL, SW_SHOW);
}

void CChildView::OnTimer(UINT_PTR nIDEvent)
{
	if (IDT_FIRST_STARTSVR == nIDEvent)
	{
		::PostMessage(GetParent()->GetSafeHwnd(), WM_FIRST_STARTSVR, 0, 0);
		KillTimer(IDT_FIRST_STARTSVR);
		return;
	}

	CWnd::OnTimer(nIDEvent);
}

void CChildView::OnDestroy()
{
	m_dbSvrMgr.stopServer();
	m_dbSvrMgr.unInitServer();
	CWnd::OnDestroy();
}

void CChildView::onUIDatabaseOpen(bool bSucc, const TCHAR* pszMsg)
{
	this->PostMessage(WM_DATABASE_OPEN, (WPARAM)bSucc);
}

void CChildView::onUIDatabaseClose()
{
	this->PostMessage(WM_DATABASE_CLOSE);
}

LRESULT CChildView::OnDatabaseOpenUIMsg(WPARAM wParam, LPARAM lParam)
{
	bool bSucc = (bool)wParam;

	if (bSucc)
		AppendServerMsg(_T("连接数据库成功！\r\n"));
	else
		AppendServerMsg(_T("连接数据库失败！\r\n"));

	return 0;
}

LRESULT CChildView::OnDatabaseCloseUIMsg(WPARAM wParam, LPARAM lParam)
{
	AppendServerMsg(_T("关闭数据库连接！\r\n"));
	return 0;
}

void CChildView::AppendServerMsg(const TCHAR* pszMsg)
{
	int nLength = m_msgEdit.SendMessage(WM_GETTEXTLENGTH);
	m_msgEdit.SetSel(nLength, nLength);
	m_msgEdit.ReplaceSel(pszMsg);
}
