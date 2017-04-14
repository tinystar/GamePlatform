#include "stdafx.h"
#include "TestServer.h"
#include "TestServerDefs.h"

using namespace std;


TestServer::TestServer()
	: m_bRunning(false)
{
	m_pTcpService = createTcpService();
	if (m_pTcpService)
		m_pTcpService->addEventHandler(this);

	m_szFileName[0] = 0;
}

TestServer::~TestServer()
{
	if (m_pTcpService)
		releaseTcpService(m_pTcpService);
}

bool TestServer::start()
{
	if (m_bRunning)
		return true;

	if (NULL == m_pTcpService)
		return false;

	SVCErrorCode ec = eOk;

	TcpConfig config;
	config.sPort = TEST_SERVER_PORT;
	config.nMaxPackageSize = MAX_PACKAGE_SIZE;
	ec = m_pTcpService->init(config);
	if (ec != eOk)
		return false;

	ec = m_pTcpService->start();
	if (ec != eOk)
		return false;

	m_bRunning = true;
	return true;
}

bool TestServer::stop()
{
	if (!m_bRunning)
		return false;

	SVCErrorCode ec = eOk;

	ec = m_pTcpService->stop();
	EzAssert(ec == eOk);

	ec = m_pTcpService->unInit();
	EzAssert(ec == eOk);

	m_bRunning = false;
	return true;
}

void TestServer::onClientConnected(ClientId id)
{
}

void TestServer::onPackageReceived(ClientId id, void* pPackage, size_t nSize)
{
	TestMsgBase* pMsg = (TestMsgBase*)pPackage;
	if (NULL == pMsg)
		return;

	switch (pMsg->nMsgId)
	{
		case MsgIdEcho:
		{
			EchoMsg* pEchoMsg = (EchoMsg*)pPackage;
			m_pTcpService->sendData(id, pEchoMsg->szMsgBuf, pEchoMsg->nMsgSize);
		}
		break;
		case MsgIdUpload:
		{
			TransferFileMsg* pFileMsg = (TransferFileMsg*)pPackage;
			if (pFileMsg->nFileMsgId == FMsgIdName)
			{
				if (pFileMsg->nBufSize <= 260)
				{
					strcpy_s(m_szFileName, 260, (const char*)pFileMsg->szData);
					m_ofile.open(m_szFileName, ios::binary);
				}
			}
			else if (pFileMsg->nFileMsgId == FMsgIdData)
			{
				m_ofile.write((const char*)pFileMsg->szData, pFileMsg->nBufSize);
			}
			else if (pFileMsg->nFileMsgId == FMsgIdOver)
			{
				m_ofile.close();
				m_szFileName[0] = 0;
			}
		}
		break;
		case MsgIdDownload:
		{
			ifstream ifile;
			ifile.open("F:\\GamePlatform\\Out\\Bin\\Test\\Win32\\Debug\\1080i.avi", ios::binary);
			if (!ifile)
				return;

			const char* pDstFile = "Download\\1080i.avi";

			CSUINT32 nTotalSize = sizeof(TransferFileMsg) + 1024 - 1;
			CSBYTE* pMsgBuf = new CSBYTE[nTotalSize];
			TransferFileMsg* pTsFileMsg = (TransferFileMsg*)pMsgBuf;
			pTsFileMsg->nMsgId = MsgIdDownload;
			pTsFileMsg->nFileMsgId = FMsgIdName;
			pTsFileMsg->nBufSize = strlen(pDstFile) + 1;
			::memcpy(pTsFileMsg->szData, pDstFile, strlen(pDstFile) + 1);

			EzStopwatch watch;
			EzTrace(_T("------------before send file-------------"));
			watch.start();

			m_pTcpService->sendData(id, pTsFileMsg, sizeof(TransferFileMsg) + strlen(pDstFile));

			while (!ifile.eof())
			{
				pTsFileMsg->nFileMsgId = FMsgIdData;
				ifile.read((char*)pTsFileMsg->szData, 1024);
				pTsFileMsg->nBufSize = (CSUINT32)ifile.gcount();
				m_pTcpService->sendData(id, pTsFileMsg, sizeof(TransferFileMsg) + (size_t)ifile.gcount() - 1);
			}

			pTsFileMsg->nFileMsgId = FMsgIdOver;
			m_pTcpService->sendData(id, pTsFileMsg, sizeof(TransferFileMsg));

			EzTrace(_T("------------after send file, Total time %I64d-------------"), watch.stop());

			delete[] pMsgBuf;
		}
		break;
		case MsgIdTest:
		{

		}
		break;
	}

	return;
}

void TestServer::onClientClosed(ClientId id)
{
}

void TestServer::test()
{
	SVCErrorCode ec = m_pTcpService->stop();

	//ec = m_pTcpService->start();
}
