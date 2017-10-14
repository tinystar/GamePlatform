
// TestClientSocketDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TestClientSocket.h"
#include "TestClientSocketDlg.h"
#include "afxdialogex.h"

using namespace std;

#define ID_TIMER_SELECT		101


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


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


// CTestClientSocketDlg �Ի���



CTestClientSocketDlg::CTestClientSocketDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTestClientSocketDlg::IDD, pParent)
	, m_sockTest1(MAX_ECHOMSG_SIZE)
	, m_nFileSize(0)
	, m_nRecvSize(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_sockTest1.addEventHandler(this);
}

CTestClientSocketDlg::~CTestClientSocketDlg()
{
	m_sockTest1.removeEventHandler(this);
}

void CTestClientSocketDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_SRC, m_sSrcFile);
	DDX_Text(pDX, IDC_EDIT_DST, m_sDstFile);
	DDX_Control(pDX, IDC_EDIT_TEST1, m_editTest1);
}

BEGIN_MESSAGE_MAP(CTestClientSocketDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_TEST1, &CTestClientSocketDlg::OnTest1Clicked)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CTestClientSocketDlg ��Ϣ�������

BOOL CTestClientSocketDlg::OnInitDialog()
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
	//SetTimer(ID_TIMER_SELECT, 1, NULL);
 	HANDLE hThread = (HANDLE)::_beginthreadex(NULL, 0, selectThread, this, 0, NULL);
 	::CloseHandle(hThread);
	m_sockTest1.create(false);
	m_sockTest1.connect("127.0.0.1", 5010);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CTestClientSocketDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CTestClientSocketDlg::OnPaint()
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
HCURSOR CTestClientSocketDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CTestClientSocketDlg::OnTest1Clicked()
{
	UpdateData();

	ifstream ifile;
	ifile.open(m_sSrcFile, ios::binary);
	if (!ifile)
		return;

	ifile.seekg(0, std::ios::end);
	m_nFileSize = ifile.tellg();
	ifile.seekg(0, std::ios::beg);

	EzStopwatch watch;
	watch.start();

	char szBuffer[MAX_ECHOMSG_SIZE] = { 0 };
	while (!ifile.eof())
	{
		ifile.read(szBuffer, MAX_ECHOMSG_SIZE);
		m_sockTest1.sendData(szBuffer, (size_t)ifile.gcount());
	}

	EzTrace(_T("send file:%s spend %f seconds"), m_sSrcFile.GetString(), watch.stop());
}

void CTestClientSocketDlg::onSocketConnected(TcpClientSocket* pClientSock)
{
	if (pClientSock == &m_sockTest1)
		m_editTest1.SetWindowText(_T("Test client socket1 successful Connected!"));
}

void CTestClientSocketDlg::onSocketRecved(TcpClientSocket* pClientSock, void* pPackage, size_t nSize)
{
	if (pClientSock == &m_sockTest1)
	{
		if (0 == m_nRecvSize)
			m_recvFile.open(m_sDstFile, ios::binary);

		m_recvFile.write((const char*)pPackage, nSize);
		m_nRecvSize += nSize;
		if (m_nRecvSize >= m_nFileSize)
		{
			m_nRecvSize = 0;
			m_nFileSize = 0;
			m_recvFile.close();
		}
	}
}

void CTestClientSocketDlg::onSocketClosed(TcpClientSocket* pClientSock, int nErrCode)
{

}

void CTestClientSocketDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (ID_TIMER_SELECT == nIDEvent)
		TcpClientSocket::select(1);
}

unsigned __stdcall CTestClientSocketDlg::selectThread(void* pParam)
{
	CTestClientSocketDlg* pDlg = (CTestClientSocketDlg*)pParam;
	while (true)
	{
		pDlg->m_sockTest1.select(1);
	}

	return 0;
}

