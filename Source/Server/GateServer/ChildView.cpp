
// ChildView.cpp : CChildView 类的实现
//

#include "stdafx.h"
#include "GateServerApp.h"
#include "ChildView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define ID_BTN_START			1000
#define ID_BTN_DUMP				1001
#define ID_LCTL_MSVR			1002
#define ID_BTN_UPDCFG			1003
#define ID_BTN_OPENDIR			1004

#define IDT_FIRST_STARTSVR		110


// UI notify msg
#define WM_MAINSVR_ADD			(WM_USER + 1)
#define WM_MAINSVR_REMOVE		(WM_USER + 2)
#define WM_MAIN_USER_ADD		(WM_USER + 3)
#define WM_MAIN_USER_REMOVE		(WM_USER + 4)

struct AddServerParam
{
	ClientId			id;
	TCHAR				szMainIP[20];
	TCHAR				szMainName[64];
	unsigned short		sMainPort;
	unsigned int		uMaxUser;
	unsigned int		uOnlineUser;
};

struct RemoveServerParam
{
	ClientId			id;
};

struct MainUserParam
{
	ClientId			id;
};

// CChildView

CChildView::CChildView()
	: m_svrStatus(0)
	, m_bFirstStart(TRUE)
{
	m_gateSvrMgr.getServer().registerUIObserver(this);
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
	ON_MESSAGE(WM_MAINSVR_ADD, &CChildView::OnAddServerUIMsg)
	ON_MESSAGE(WM_MAINSVR_REMOVE, &CChildView::OnRemoveServerUIMsg)
	ON_MESSAGE(WM_MAIN_USER_ADD, &CChildView::OnMainUserLoginUIMsg)
	ON_MESSAGE(WM_MAIN_USER_REMOVE, &CChildView::OnMainUserLogoutUIMsg)
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

	CRect rcControl = { 520, 30, 620, 55 };
	m_startBtn.Create(strRes, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, rcControl, this, ID_BTN_START);
	m_startBtn.SetFont(&m_font);

	rcControl.OffsetRect(0, 35);
	m_dumpBtn.Create(_T("Dump"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, rcControl, this, ID_BTN_DUMP);
	m_dumpBtn.SetFont(&m_font);
	m_dumpBtn.EnableWindow(FALSE);

	rcControl.OffsetRect(0, 35);
	strRes.LoadString(IDS_UPDATECONFIG);
	m_updCfgBtn.Create(strRes, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, rcControl, this, ID_BTN_UPDCFG);
	m_updCfgBtn.SetFont(&m_font);
	m_updCfgBtn.EnableWindow(FALSE);

	rcControl.OffsetRect(0, 35);
	strRes.LoadString(IDS_OPENDIR);
	m_openDirBtn.Create(strRes, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, rcControl, this, ID_BTN_OPENDIR);
	m_openDirBtn.SetFont(&m_font);

	strRes.LoadString(IDS_MAINSVR_LIST);
	rcControl.SetRect(5, 10, 105, 30);
	m_msvrLabel.Create(strRes, WS_CHILD | WS_VISIBLE, rcControl, this, 0);
	m_msvrLabel.SetFont(&m_font);

	rcControl.SetRect(5, 30, 505, 418);
	m_msvrList.Create(WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT, rcControl, this, ID_LCTL_MSVR);
	strRes.LoadString(IDS_SERVER_ADDR);
	m_msvrList.InsertColumn(0, strRes, LVCFMT_LEFT, 120);
	strRes.LoadString(IDS_SERVER_PORT);
	m_msvrList.InsertColumn(1, strRes, LVCFMT_LEFT, 80);
	strRes.LoadString(IDS_SERVER_NAME);
	m_msvrList.InsertColumn(2, strRes, LVCFMT_LEFT, 120);
	strRes.LoadString(IDS_ONLINE_COUNT);
	m_msvrList.InsertColumn(3, strRes, LVCFMT_LEFT, 80);
	strRes.LoadString(IDS_SERVER_USERCNT);
	m_msvrList.InsertColumn(4, strRes, LVCFMT_LEFT, 80);

	SVCErrorCode ec = eOk;
	if ((ec = m_gateSvrMgr.initServer()) != eOk)
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
		if ((ec = m_gateSvrMgr.stopServer()) != eOk)
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
		if ((ec = m_gateSvrMgr.startServer()) != eOk)
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
	m_gateSvrMgr.stopServer();
	m_gateSvrMgr.unInitServer();

	for (int i = 0; i < m_msvrList.GetItemCount(); ++i)
	{
		AddServerParam* pAddParam = (AddServerParam*)m_msvrList.GetItemData(i);
		delete pAddParam;
	}

	CWnd::OnDestroy();
}

void CChildView::onUIMainServerAdded(ClientId id, const MainSvrNode& svrNode)
{
	AddServerParam* pAddParam = new AddServerParam();
	EzString sMainIP(svrNode.szMainAddr, kAnsi);
	EzString sMainName(svrNode.szMainName, kUtf8);
	memcpy(pAddParam->szMainIP, sMainIP.kwcharPtr(), sizeof(pAddParam->szMainIP));
	memcpy(pAddParam->szMainName, sMainName.kwcharPtr(), sizeof(pAddParam->szMainName));

	pAddParam->id = id;
	pAddParam->sMainPort = svrNode.sMainPort;
	pAddParam->uMaxUser = svrNode.uMaxUser;
	pAddParam->uOnlineUser = svrNode.uOnlineUser;

	this->PostMessage(WM_MAINSVR_ADD, 0, (LPARAM)pAddParam);
}

void CChildView::onUIMainServerRemoved(ClientId id)
{
	RemoveServerParam* pRemoveParam = new RemoveServerParam();
	pRemoveParam->id = id;

	this->PostMessage(WM_MAINSVR_REMOVE, 0, (LPARAM)pRemoveParam);
}

void CChildView::onUIMainUserLogin(ClientId id)
{
	MainUserParam* pParam = new MainUserParam();
	pParam->id = id;

	this->PostMessage(WM_MAIN_USER_ADD, 0, (LPARAM)pParam);
}

void CChildView::onUIMainUserLogout(ClientId id)
{
	MainUserParam* pParam = new MainUserParam();
	pParam->id = id;

	this->PostMessage(WM_MAIN_USER_REMOVE, 0, (LPARAM)pParam);
}

LRESULT CChildView::OnAddServerUIMsg(WPARAM wParam, LPARAM lParam)
{
	AddServerParam* pAddParam = (AddServerParam*)lParam;

	int idx = m_msvrList.GetItemCount();
	idx = m_msvrList.InsertItem(idx, NULL);
	m_msvrList.SetItemData(idx, (DWORD_PTR)pAddParam);

	m_msvrList.SetItemText(idx, 0, pAddParam->szMainIP);
	TCHAR szBuf[256] = { 0 };
	_stprintf_s(szBuf, 256, _T("%d"), pAddParam->sMainPort);
	m_msvrList.SetItemText(idx, 1, szBuf);
	m_msvrList.SetItemText(idx, 2, pAddParam->szMainName);
	_stprintf_s(szBuf, 256, _T("%d"), pAddParam->uOnlineUser);
	m_msvrList.SetItemText(idx, 3, szBuf);
	_stprintf_s(szBuf, 256, _T("%d"), pAddParam->uMaxUser);
	m_msvrList.SetItemText(idx, 4, szBuf);

	return 0;
}

LRESULT CChildView::OnRemoveServerUIMsg(WPARAM wParam, LPARAM lParam)
{
	RemoveServerParam* pRemoveParam = (RemoveServerParam*)lParam;

	for (int i = 0; i < m_msvrList.GetItemCount(); ++i)
	{
		AddServerParam* pAddParam = (AddServerParam*)m_msvrList.GetItemData(i);
		if (pAddParam->id == pRemoveParam->id)
		{
			m_msvrList.DeleteItem(i);
			delete pAddParam;
			break;
		}
	}

	delete pRemoveParam;
	return 0;
}

LRESULT CChildView::OnMainUserLoginUIMsg(WPARAM wParam, LPARAM lParam)
{
	MainUserParam* pParam = (MainUserParam*)lParam;

	updateOnlineUserDisplay(pParam->id, true);
	delete pParam;
	return 0;
}

LRESULT CChildView::OnMainUserLogoutUIMsg(WPARAM wParam, LPARAM lParam)
{
	MainUserParam* pParam = (MainUserParam*)lParam;

	updateOnlineUserDisplay(pParam->id, false);
	delete pParam;
	return 0;
}

void CChildView::updateOnlineUserDisplay(ClientId svrId, bool bLogin)
{
	for (int i = 0; i < m_msvrList.GetItemCount(); ++i)
	{
		AddServerParam* pAddParam = (AddServerParam*)m_msvrList.GetItemData(i);
		if (pAddParam->id == svrId)
		{
			bLogin ? pAddParam->uOnlineUser++ : pAddParam->uOnlineUser--;
			TCHAR szBuf[256] = { 0 };
			_stprintf_s(szBuf, 256, _T("%d"), pAddParam->uOnlineUser);
			m_msvrList.SetItemText(i, 3, szBuf);
			break;
		}
	}
}
