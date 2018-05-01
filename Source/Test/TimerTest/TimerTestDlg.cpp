
// TimerTestDlg.cpp : ʵ���ļ�
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

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CTimerTestDlg �Ի���



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


// CTimerTestDlg ��Ϣ�������

BOOL CTimerTestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO:  �ڴ���Ӷ���ĳ�ʼ������
	MinHeap<int> testheap;
	testheap.insert(53);
	testheap.insert(17);
	testheap.insert(78);
	testheap.insert(9);
	testheap.insert(45);
	testheap.insert(65);
	testheap.insert(87);
	testheap.insert(23);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CTimerTestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
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
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);

	TIMECAPS tc;
	if (timeGetDevCaps(&tc, sizeof(TIMECAPS)) != TIMERR_NOERROR)
	{

	}

	UINT wTimerRes = min(max(tc.wPeriodMin, 1), tc.wPeriodMax);
	timeBeginPeriod(wTimerRes);

	// ����ֻ֧��16��
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
