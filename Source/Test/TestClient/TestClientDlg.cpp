
// TestClientDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TestClient.h"
#include "TestClientDlg.h"
#include "afxdialogex.h"
#include "TestServerDefs.h"
#include <fstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;

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


// CTestClientDlg 对话框



CTestClientDlg::CTestClientDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTestClientDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTestClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTestClientDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_SEND, OnSendClicked)
	ON_BN_CLICKED(IDC_BUTTON_TEST, OnTestClicked)
	ON_BN_CLICKED(IDC_BTN_UPLOAD, OnUploadClicked)
	ON_BN_CLICKED(IDC_BTN_DOWNLOAD, OnDownloadClicked)
END_MESSAGE_MAP()


// CTestClientDlg 消息处理程序

BOOL CTestClientDlg::OnInitDialog()
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
	bool bRet = m_testClient.connectToServer("127.0.0.1", TEST_SERVER_PORT);
	bRet = m_uploadSock.connectToServer("127.0.0.1", TEST_SERVER_PORT);
	bRet = m_downloadSock.connectToServer("127.0.0.1", TEST_SERVER_PORT);

	ASSERT(bRet);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CTestClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CTestClientDlg::OnPaint()
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
HCURSOR CTestClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CTestClientDlg::OnDestroy()
{
	m_testClient.close();
	m_uploadSock.close();
	m_downloadSock.close();

	CDialogEx::OnDestroy();
}

void CTestClientDlg::OnSendClicked()
{
	CEdit* pEditor = (CEdit*)GetDlgItem(IDC_EDITOR);
	ASSERT(pEditor);
	CString strInput;
	pEditor->GetWindowText(strInput);

	if (!strInput.IsEmpty())
	{
		CEdit* pHistory = (CEdit*)GetDlgItem(IDC_HISTORY);
		ASSERT(pHistory);
		CString strTalk;
		pHistory->GetWindowText(strTalk);

		if (!strTalk.IsEmpty())
			strTalk += _T("\r\n");
		strTalk += _T("Client:");
		strTalk += strInput;

		pHistory->SetWindowText(strTalk);
		pEditor->SetWindowText(_T(""));

		int nMsgSize = (strInput.GetLength() + 1) * 2;
		CSBYTE* pEchoBuf = new CSBYTE[sizeof(EchoMsg) + nMsgSize - 1];
		EchoMsg* pEchoMsg = (EchoMsg*)pEchoBuf;
		pEchoMsg->nMsgId = MsgIdEcho;
		pEchoMsg->nMsgSize = nMsgSize;
		::memcpy(pEchoMsg->szMsgBuf, strInput.GetString(), nMsgSize);
		bool bRet = m_testClient.sendPackage((const char*)pEchoMsg, sizeof(EchoMsg) + nMsgSize - 1);
		ASSERT(bRet);
		delete[] pEchoBuf;

		char szRecvBuf[1024] = { 0 };
		bRet = m_testClient.recvPackage(szRecvBuf, 1024);
		ASSERT(bRet);
		
		CString strRecv((LPCTSTR)szRecvBuf);
		if (!strRecv.IsEmpty())
		{
			if (!strTalk.IsEmpty())
				strTalk += _T("\r\n");
			strTalk += _T("Server:");
			strTalk += strRecv;
			pHistory->SetWindowText(strTalk);
		}
	}
}

ClientSocket g_testClient2;

unsigned __stdcall testSocketThread(void* pParam)
{
	//ClientSocket* pClient = (ClientSocket*)pParam;
	ClientSocket testSocket;
	ClientSocket* pClient = (ClientSocket*)&testSocket;
	if (pClient)
	{
		pClient->connectToServer("127.0.0.1", TEST_SERVER_PORT);
		unsigned int nSeq = 0;
		while (true)
		{
			++nSeq;
			pClient->sendPackage((const char*)&nSeq, sizeof(nSeq));
			//::Sleep(1);
			if (3 == nSeq)
			{
				break;
// 				char szBuffer[1024] = { 0 };
// 				pClient->recvPackage(szBuffer, 1024);
// 				if (1000 == *((int*)szBuffer))
// 				{
// 					pClient->sendPackage("test send", 9);
// 					OutputDebugString(_T("client recved 1000!\n"));
// 					::Sleep(1);
// 					g_testClient2.connectToServer("127.0.0.1", TEST_SERVER_PORT);
// 					bool bRet = g_testClient2.recvPackage(szBuffer, 1024);
// 					if (!bRet)
// 					{
// 						::MessageBox(NULL, _T("recv fail!"), _T("error"), MB_OK);
// 					}
// 				}
			}
		}
		pClient->close();
	}
	return 0;
}

void CTestClientDlg::OnTestClicked()
{
// 	for (int i = 0; i < 500; ++i)
// 	{
// 		HANDLE hThread = (HANDLE)::_beginthreadex(NULL, 0, testSocketThread, NULL, 0, NULL);
// 		::CloseHandle(hThread);
// 	}
	ClientSocket testSocket;
	testSocket.connectToServer("127.0.0.1", TEST_SERVER_PORT);
	for (int i = 0; i < 5; ++i)
		testSocket.sendPackage("test client!!!", 14);
	testSocket.sendPackage("close", 5);
	char szRecvBuf[512] = { 0 };
	testSocket.recvPackage(szRecvBuf, 512);
	testSocket.recvPackage(szRecvBuf, 512);
// 	testSocket.recvPackage(szRecvBuf, 512);
// 	testSocket.recvPackage(szRecvBuf, 512);
	for (int i = 0; i < 5; ++i)
		testSocket.sendPackage("continue send!", strlen("continue send!"));
}

bool sendFileMsg(ClientSocket& cSocket, CSUINT8 nMainId, CSUINT8 nSubId, const char* pMsg, CSUINT32 nMsgSize)
{
	CSUINT32 nTotalSize = sizeof(TransferFileMsg) + nMsgSize - 1;
	CSBYTE* pMsgBuf = new CSBYTE[nTotalSize];
	TransferFileMsg* pTsFileMsg = (TransferFileMsg*)pMsgBuf;
	pTsFileMsg->nMsgId = nMainId;
	pTsFileMsg->nFileMsgId = nSubId;
	pTsFileMsg->nBufSize = nMsgSize;
	::memcpy(pTsFileMsg->szData, pMsg, nMsgSize);

	bool bRet = cSocket.sendPackage((const char*)pTsFileMsg, nTotalSize);
	//ASSERT(bRet);
	delete[] pMsgBuf;

	return bRet;
}

char* wideCharToUtf8(const wchar_t* pSrc)
{
	if (NULL == pSrc)
		return NULL;

	int len = ::WideCharToMultiByte(CP_UTF8, 0, pSrc, -1, NULL, 0, NULL, NULL);
	char* pszResult = new char[len];
	if (!pszResult)
		return NULL;

	int cLen = ::WideCharToMultiByte(CP_UTF8, 0, pSrc, -1, pszResult, len, NULL, NULL);
	if (cLen <= 0)
	{
		delete pszResult;
		return NULL;
	}

	return pszResult;
}

void CTestClientDlg::OnUploadClicked()
{
	CWnd* pUploadEdit = GetDlgItem(IDC_EDIT_UPLOAD);
	if (NULL == pUploadEdit)
		return;

	CString sFileName;
	pUploadEdit->GetWindowText(sFileName);
	if (sFileName.IsEmpty())
		return;

	char* pFileName = wideCharToUtf8(sFileName.GetString());

	ifstream ifile;
	ifile.open(pFileName, ios::binary);

	char szDrive[256] = { 0 };
	char szDir[256] = { 0 };
	char szFile[256] = { 0 };
	char szExt[256] = { 0 };
	_splitpath_s(pFileName, szDrive, szDir, szFile, szExt);
	strcat_s(szFile, 256, szExt);
	delete pFileName;

	if (!ifile)
		return;

	sendFileMsg(m_uploadSock, MsgIdUpload, FMsgIdName, szFile, strlen(szFile) + 1);

	char szBuffer[1024] = { 0 };
	while (!ifile.eof())
	{
		ifile.read(szBuffer, 1024);
		sendFileMsg(m_uploadSock, MsgIdUpload, FMsgIdData, szBuffer, (CSUINT32)ifile.gcount());
	}

	sendFileMsg(m_uploadSock, MsgIdUpload, FMsgIdOver, NULL, 0);
}

void CTestClientDlg::OnDownloadClicked()
{
	TransferFileMsg tfileMsg;
	tfileMsg.nMsgId = MsgIdDownload;
	m_downloadSock.sendPackage((const char*)&tfileMsg, sizeof(tfileMsg));

	char szBuffer[1040] = { 0 };
	bool bRet = m_downloadSock.recvPackage(szBuffer, 1040);
	if (!bRet)
		return;

	TransferFileMsg* pFileMsg = (TransferFileMsg*)szBuffer;
	ofstream ofile((const char*)pFileMsg->szData, ios::binary);
	if (!ofile)
		return;

	do 
	{
		bRet = m_downloadSock.recvPackage(szBuffer, 1040);
		TransferFileMsg* pFileMsg = (TransferFileMsg*)szBuffer;
		if (pFileMsg->nFileMsgId == FMsgIdData)
			ofile.write((char*)pFileMsg->szData, pFileMsg->nBufSize);
		else if (pFileMsg->nFileMsgId == FMsgIdOver)
		{
			ofile.close();
			break;
		}
	} while (true);
}

