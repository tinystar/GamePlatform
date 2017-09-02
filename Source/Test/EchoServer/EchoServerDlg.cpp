
// EchoServerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "EchoServer.h"
#include "EchoServerDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CEchoServerDlg 对话框



CEchoServerDlg::CEchoServerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CEchoServerDlg::IDD, pParent)
	, m_pClientList(NULL)
{
	m_echoServer.registerUIObserver(this);
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CEchoServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CEchoServerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_MESSAGE(WM_CLIENT_CONNECT, &CEchoServerDlg::OnClientConnUIMsg)
	ON_MESSAGE(WM_CLIENT_CLOSE, &CEchoServerDlg::OnClientCloseUIMsg)
END_MESSAGE_MAP()


// CEchoServerDlg 消息处理程序

BOOL CEchoServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化代码
	m_pClientList = (CListCtrl*)GetDlgItem(IDC_CLIENT_LIST);
	if (NULL == m_pClientList)
	{
		AfxMessageBox(_T("get client list control failed!"));
		return TRUE;
	}

	m_pClientList->InsertColumn(0, _T("num"), LVCFMT_LEFT, 40);
	m_pClientList->InsertColumn(1, _T("ip"), LVCFMT_LEFT, 150);
	m_pClientList->InsertColumn(2, _T("port"), LVCFMT_LEFT, 50);

	ServerInitConfig config;
	config.uCreateFlags = kCreateTcpService;
	config.tcpConfig.sPort = ECHO_SERVER_PORT;
	config.tcpConfig.nMaxPackageSize = MAX_ECHOMSG_SIZE;
	m_echoServer.init(config);
	m_echoServer.start();
	config.uCreateFlags = kCreateAllServices;
	config.tcpConfig.sPort = TIME_SERVER_PORT;
	config.tcpConfig.nMaxPackageSize = MAX_TIMEMSG_SIZE;
	m_timeServer.init(config);
	m_timeServer.start();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CEchoServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CEchoServerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CEchoServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CEchoServerDlg::OnDestroy()
{
	m_echoServer.stop();
	m_echoServer.unInit();
	m_timeServer.stop();
	m_timeServer.unInit();
}

// 此回调函数是在工作线程中响应，需要发消息给主线程来更新界面，否则可能死锁
void CEchoServerDlg::onClientConnect(ClientId id)
{
	this->PostMessage(WM_CLIENT_CONNECT, 0, (LPARAM)(ClientContext*)id);
}

void CEchoServerDlg::onClientClose(ClientId id)
{
	this->PostMessage(WM_CLIENT_CLOSE, 0, (LPARAM)(ClientContext*)id);
}

LRESULT CEchoServerDlg::OnClientConnUIMsg(WPARAM wParam, LPARAM lParam)
{
	ClientId id( (ClientContext*)lParam );
	if (id.isNull())
		return 0;

	int idx = m_pClientList->GetItemCount();

	TCHAR szBuf[256] = { 0 };
	_stprintf_s(szBuf, 256, _T("%d"), idx + 1);
	idx = m_pClientList->InsertItem(idx, szBuf);
	m_pClientList->SetItemData(idx, (DWORD_PTR)lParam);

	SOCKADDR_IN clientAddr = id.getAddress();
	char* ip = inet_ntoa(clientAddr.sin_addr);
	wchar_t* ipAddr = EzText::ansiToWideChar(ip);
	m_pClientList->SetItemText(idx, 1, ipAddr);
	delete ipAddr;

	_stprintf_s(szBuf, 256, _T("%d"), clientAddr.sin_port);
	m_pClientList->SetItemText(idx, 2, szBuf);

	return 0;
}

LRESULT CEchoServerDlg::OnClientCloseUIMsg(WPARAM wParam, LPARAM lParam)
{
	for (int i = 0; i < m_pClientList->GetItemCount(); ++i)
	{
		if (m_pClientList->GetItemData(i) == (DWORD_PTR)lParam)
		{
			m_pClientList->DeleteItem(i);
			break;
		}
	}

	return 0;
}
