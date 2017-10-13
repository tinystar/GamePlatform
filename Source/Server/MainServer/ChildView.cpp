
// ChildView.cpp : CChildView 类的实现
//

#include "stdafx.h"
#include "MainServerApp.h"
#include "ChildView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define ID_BTN_START			1000
#define ID_BTN_DUMP				1001
#define ID_BTN_UPDCFG			1002
#define ID_BTN_OPENDIR			1003

#define IDT_FIRST_STARTSVR		110


// CChildView

CChildView::CChildView()
	: m_svrStatus(0)
	, m_bFirstStart(TRUE)
{
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

	CRect rcControl = { 20, 390, 120, 415 };
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

	rcControl.SetRect(25, 300, 50, 320);
	strRes.LoadString(IDS_PORT);
	m_portLabel.Create(strRes, WS_CHILD | WS_VISIBLE, rcControl, this);
	m_portLabel.SetFont(&m_font);

	rcControl.SetRect(52, 298, 97, 318);
	m_portEdit.Create(WS_CHILD | WS_VISIBLE | WS_BORDER, rcControl, this, -1);

	rcControl.SetRect(117, 300, 192, 320);
	strRes.LoadString(IDS_MAX_USER);
	m_maxUserLabel.Create(strRes, WS_CHILD | WS_VISIBLE, rcControl, this);
	m_maxUserLabel.SetFont(&m_font);

	rcControl.SetRect(194, 298, 254, 318);
	m_maxUserEdit.Create(WS_CHILD | WS_VISIBLE | WS_BORDER, rcControl, this, -1);

	rcControl.SetRect(25, 330, 110, 350);
	strRes.LoadString(IDS_GATE_ADDR);
	m_gateAddrLabel.Create(strRes, WS_CHILD | WS_VISIBLE, rcControl, this);
	m_gateAddrLabel.SetFont(&m_font);

	rcControl.SetRect(110, 328, 250, 348);
	m_gateAddrEdit.Create(WS_CHILD | WS_VISIBLE | WS_BORDER, rcControl, this, -1);

	rcControl.SetRect(270, 330, 355, 350);
	strRes.LoadString(IDS_GATE_PORT);
	m_gatePortLabel.Create(strRes, WS_CHILD | WS_VISIBLE, rcControl, this);
	m_gatePortLabel.SetFont(&m_font);

	rcControl.SetRect(355, 328, 400, 348);
	m_gatePortEdit.Create(WS_CHILD | WS_VISIBLE | WS_BORDER, rcControl, this, -1);

	rcControl.SetRect(25, 360, 110, 380);
	strRes.LoadString(IDS_DB_ADDR);
	m_DBAddrLabel.Create(strRes, WS_CHILD | WS_VISIBLE, rcControl, this);
	m_DBAddrLabel.SetFont(&m_font);

	rcControl.SetRect(110, 358, 250, 378);
	m_DBAddrEdit.Create(WS_CHILD | WS_VISIBLE | WS_BORDER, rcControl, this, -1);

	rcControl.SetRect(270, 360, 355, 380);
	strRes.LoadString(IDS_DB_PORT);
	m_DBPortLabel.Create(strRes, WS_CHILD | WS_VISIBLE, rcControl, this);
	m_DBPortLabel.SetFont(&m_font);

	rcControl.SetRect(355, 358, 400, 378);
	m_DBPortEdit.Create(WS_CHILD | WS_VISIBLE | WS_BORDER, rcControl, this, -1);

	if (!m_mainSvrMgr.loadConfig())
	{
		MessageBox(_T("load config failed\n"));
		m_startBtn.EnableWindow(FALSE);
	}
	else
	{
		CString strPort;
		strPort.Format(_T("%d"), m_mainSvrMgr.getPort());
		m_portEdit.SetWindowText(strPort);

		strPort.Format(_T("%d"), m_mainSvrMgr.getMaxUser());
		m_maxUserEdit.SetWindowText(strPort);

		char szAddr[50] = { 0 };
		unsigned short sPort = 0;
#ifdef _UNICODE
		m_mainSvrMgr.getGateSvrAddr(szAddr, 50, sPort);
		wchar_t* pAddrW = EzText::ansiToWideChar(szAddr);
		m_gateAddrEdit.SetWindowText(pAddrW);
		delete pAddrW;
		strPort.Format(_T("%d"), sPort);
		m_gatePortEdit.SetWindowText(strPort);

		m_mainSvrMgr.getDBSvrAddr(szAddr, 50, sPort);
		pAddrW = EzText::ansiToWideChar(szAddr);
		m_DBAddrEdit.SetWindowText(pAddrW);
		delete pAddrW;
		strPort.Format(_T("%d"), sPort);
		m_DBPortEdit.SetWindowText(strPort);
#else
		m_mainSvrMgr.getGateSvrAddr(szAddr, 50, sPort);
		m_gateAddrEdit.SetWindowText(szAddr);
		strPort.Format(_T("%d"), sPort);
		m_gatePortEdit.SetWindowText(strPort);

		m_mainSvrMgr.getDBSvrAddr(szAddr, 50, sPort);
		m_DBAddrEdit.SetWindowText(szAddr);
		strPort.Format(_T("%d"), sPort);
		m_DBPortEdit.SetWindowText(strPort);
#endif
	}

	return 0;
}

void CChildView::OnBtnStartClick()
{
	SVCErrorCode ec = eOk;
	if (!(m_svrStatus & kInited))
	{
		CString sPort;
		m_portEdit.GetWindowText(sPort);
		if (sPort.IsEmpty())
		{
			MessageBox(_T("server port can't be empty\n"));
			return;
		}

		unsigned short port = (unsigned short)_ttoi(sPort.GetString());
		m_mainSvrMgr.setPort(port);

		m_maxUserEdit.GetWindowText(sPort);
		unsigned int uMaxUser = (unsigned int)_ttoi(sPort.GetString());
		m_mainSvrMgr.setMaxUser(uMaxUser);

		if ((ec = m_mainSvrMgr.initServer()) != eOk)
		{
			TCHAR szLog[512] = { 0 };
			_stprintf_s(szLog, 511, _T("server init failed, error code = %d\n"), ec);
			MessageBox(szLog);
			m_startBtn.EnableWindow(FALSE);
			return;
		}
		else
		{
			m_svrStatus |= kInited;
			m_portEdit.EnableWindow(FALSE);
			m_maxUserEdit.EnableWindow(FALSE);
		}
	}

	if (m_svrStatus & kStarted)
	{
		if ((ec = m_mainSvrMgr.stopServer()) != eOk)
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
			m_gateAddrEdit.EnableWindow(TRUE);
			m_gatePortEdit.EnableWindow(TRUE);
			m_DBAddrEdit.EnableWindow(TRUE);
			m_DBPortEdit.EnableWindow(TRUE);
		}
	}
	else
	{
		CString sGateAddr, sGatePort, sDBAddr, sDBPort;
		m_gateAddrEdit.GetWindowText(sGateAddr);
		m_gatePortEdit.GetWindowText(sGatePort);
		m_DBAddrEdit.GetWindowText(sDBAddr);
		m_DBPortEdit.GetWindowText(sDBPort);
		if (sGateAddr.IsEmpty() || sGatePort.IsEmpty() ||
			sDBAddr.IsEmpty() || sDBPort.IsEmpty())
		{
			MessageBox(_T("please input valid gate server address and db server address\n"));
			return;
		}

#ifdef _UNICODE
		char* pAddrA = EzText::wideCharToAnsi(sGateAddr.GetString());
		unsigned short port = (unsigned short)_ttoi(sGatePort.GetString());
		m_mainSvrMgr.setGateSvrAddr(pAddrA, port);
		delete pAddrA;

		pAddrA = EzText::wideCharToAnsi(sDBAddr.GetString());
		port = (unsigned short)_ttoi(sDBPort.GetString());
		m_mainSvrMgr.setDBSvrAddr(pAddrA, port);
		delete pAddrA;
#else
		unsigned short port = (unsigned short)_ttoi(sGatePort.GetString());
		m_mainSvrMgr.setGateSvrAddr(sGateAddr.GetString(), port);

		port = (unsigned short)_ttoi(sDBPort.GetString());
		m_mainSvrMgr.setDBSvrAddr(sDBAddr.GetString(), port);
#endif

		if ((ec = m_mainSvrMgr.startServer()) != eOk)
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
			m_gateAddrEdit.EnableWindow(FALSE);
			m_gatePortEdit.EnableWindow(FALSE);
			m_DBAddrEdit.EnableWindow(FALSE);
			m_DBPortEdit.EnableWindow(FALSE);

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
	m_mainSvrMgr.stopServer();
	m_mainSvrMgr.unInitServer();
	CWnd::OnDestroy();
}
