
// TimerTestDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TimerTest.h"
#include "TimerTestDlg.h"
#include "afxdialogex.h"
#include <mmsystem.h>
#include "xEzUtil.h"
#include "MiniHeap.h"

#pragma comment(lib, "winmm.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define UPDATE_INTERVAL_MSG		WM_USER + 1

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


// CTimerTestDlg 对话框



CTimerTestDlg::CTimerTestDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTimerTestDlg::IDD, pParent)
	, m_dInterval(0.0)
	, m_uDuration(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTimerTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_INPUT, m_uDuration);
	DDX_Text(pDX, IDC_EDIT_OUTPUT, m_dInterval);
}

BEGIN_MESSAGE_MAP(CTimerTestDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CTimerTestDlg::OnBnClickedButton1)
	ON_MESSAGE(UPDATE_INTERVAL_MSG, &CTimerTestDlg::OnUpdateInterval)
END_MESSAGE_MAP()


// CTimerTestDlg 消息处理程序

BOOL CTimerTestDlg::OnInitDialog()
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
	MinHeap<int> testheap;
	testheap.insert(53);
	testheap.insert(17);
	testheap.insert(78);
	testheap.insert(9);
	testheap.insert(45);
	testheap.insert(65);
	testheap.insert(87);
	testheap.insert(23);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CTimerTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CTimerTestDlg::OnPaint()
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
HCURSOR CTimerTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

EzStopwatch watch;
void CALLBACK TimerCallBack(UINT uTimerId, UINT msg, DWORD dwUser, DWORD dw1, DWORD dw2)
{
	CTimerTestDlg* pDlg = (CTimerTestDlg*)dwUser;

	UINT uInterval = watch.stop() * 1000 * 1000;
	pDlg->PostMessage(UPDATE_INTERVAL_MSG, 0, (LPARAM)uInterval);
	watch.start();
}

void CTimerTestDlg::OnBnClickedButton1()
{
	// TODO:  在此添加控件通知处理程序代码
	UpdateData(TRUE);

	TIMECAPS tc;
	if (timeGetDevCaps(&tc, sizeof(TIMECAPS)) != TIMERR_NOERROR)
	{

	}

	UINT wTimerRes = min(max(tc.wPeriodMin, 1), tc.wPeriodMax);
	timeBeginPeriod(wTimerRes);

	// 限制只支持16个
	for (int i = 0; i < 17; ++i)
	{
		MMRESULT mmrst = timeSetEvent(m_uDuration, 1, TimerCallBack, (DWORD_PTR)this, TIME_PERIODIC);
		if (NULL == mmrst)
		{
			DWORD ewError = GetLastError();
			MessageBox(_T("set time event failed."));
		}
	}

	watch.start();
}

LRESULT CTimerTestDlg::OnUpdateInterval(WPARAM wParam, LPARAM lParam)
{
	m_dInterval = (double)((UINT)lParam / 1000.0);
	UpdateData(FALSE);
	return 1;
}
