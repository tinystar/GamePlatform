#include "IocpRecvBuffer.h"
#include "IoContext.h"
#include "ClientContext.h"
#include "TcpPackage.h"

#define ERR_RECV_COMPLETED		-100


bool IocpRecvBuffer::init(size_t nMaxPkgSize)
{
	if (!EzVerify(nMaxPkgSize > 0))
		return false;

	m_nMaxPkgSize = nMaxPkgSize;

	m_nBufferSize = calcBufferSize(m_nMaxPkgSize);
	m_pBuffer = new char[m_nBufferSize];

	return m_pBuffer != NULL;
}

bool IocpRecvBuffer::asyncRecv(ClientContext* pClient, PER_IO_CONTEXT* pIoContext, IClientSessionMgr* pClientMgr)
{
	if (!EzVerify(pClient != NULL && pIoContext != NULL && pClientMgr != NULL))
		return false;

	if (!EzVerify(m_pBuffer != NULL))
		return false;

	pIoContext->wsaBuffer.buf = m_pBuffer + m_nRecvedSize;
	pIoContext->wsaBuffer.len = 0;		// zero bytes
	pIoContext->type = eOpRecv;

	DWORD dwTransferred = 0, dwFlags = 0;
	int iRet = ::WSARecv(pClient->getSocket(), &pIoContext->wsaBuffer, 1, &dwTransferred, &dwFlags, &pIoContext->overlapped, NULL);
	if (iRet != 0)
	{
		int iErrCode = ::WSAGetLastError();
		if (iErrCode != WSA_IO_PENDING)
		{
			EzLogError(_T("WSARecv fails in %s, line %d.\n"), _T(__FUNCTION__), __LINE__);
			pClientMgr->OnErrorOccurredNotify(pClient, eSystemError, iErrCode);
			return false;
		}
	}

	return true;
}

bool IocpRecvBuffer::onRecvCompleted(ClientContext* pClient, IClientSessionMgr* pClientMgr)
{
	if (!EzVerify(pClient != NULL && pClientMgr != NULL))
		return false;

	if (!EzVerify(m_pBuffer != NULL))
		return false;

	while (true)
	{
		if (m_nRecvedSize >= sizeof(TcpPackageHeader))	// 接收数据超过一个包头
		{
			TcpPackageHeader* pPkgHeader = (TcpPackageHeader*)m_pBuffer;
			if (pPkgHeader->uPackageSize > m_nMaxPkgSize)
			{
				EzLogError(_T("onRecvCompleted: client[%x] the receive package size is beyond the max package size .\n"), pClient);
				pClientMgr->OnErrorOccurredNotify(pClient, eInvalidPackage, 0);
				return false;
			}

			size_t nOldPkgSize = m_nRecvedSize - sizeof(TcpPackageHeader);
			EzAssert(nOldPkgSize < pPkgHeader->uPackageSize);
			int iRet = safeRecv(pClient, pPkgHeader->uPackageSize - nOldPkgSize, pClientMgr);
			if (iRet > 0)
			{
				m_nRecvedSize += iRet;

				size_t nTotalPkgSize = sizeof(TcpPackageHeader) + pPkgHeader->uPackageSize;
				if (m_nRecvedSize >= nTotalPkgSize)
				{
					pClientMgr->OnPackageRecvedNotify(pClient, pPkgHeader + 1, pPkgHeader->uPackageSize);
					if (ClientContext::kClosed == pClient->getStatus())
					{
						return false;	// OnPackageRecvedNotify通知的时候客户代码可能关闭连接，此时必须退出接收过程
					}

					if (m_nRecvedSize > nTotalPkgSize)
					{
						// 理论上不会进到这里，加上判断以防万一
						EzLogDebug(_T("it should not be here! func: %s, line: %d\n"), _T(__FUNCTION__), __LINE__);
						::memmove(m_pBuffer, m_pBuffer + nTotalPkgSize, m_nRecvedSize - nTotalPkgSize);
					}
					m_nRecvedSize -= nTotalPkgSize;
				}
			}
			else if (ERR_RECV_COMPLETED == iRet)
			{
				break;
			}
			else    // error occurred break recv
			{
				return false;
			}
		}
		else // 接收数据不够一个包头，则接收到一个完整包头
		{
			int iRet = safeRecv(pClient, sizeof(TcpPackageHeader) - m_nRecvedSize, pClientMgr);
			if (iRet > 0)
			{
				m_nRecvedSize += iRet;
			}
			else if (ERR_RECV_COMPLETED == iRet)
			{
				break;
			}
			else    // error occurred break recv
			{
				return false;
			}
		}
	}

	return true;
}

size_t IocpRecvBuffer::calcBufferSize(size_t nPkgSize)
{
	if (nPkgSize < 512)
		return 1024;

	return 2 * nPkgSize;
}

int IocpRecvBuffer::safeRecv(ClientContext* pClient, int iRecvSize, IClientSessionMgr* pClientMgr)
{
	if (!EzVerify(pClient != NULL && pClientMgr != NULL))
		return 0;

	if (!EzVerify(iRecvSize > 0))
		return 0;

	if (!EzVerify(m_pBuffer != NULL))
		return 0;

	if (m_nRecvedSize + iRecvSize > m_nBufferSize)
	{
		EzLogError(_T("the receive buffer is not enough, it should not be here!\n"));
		pClientMgr->OnErrorOccurredNotify(pClient, eBufNotEnough, 0);
		return 0;
	}

	int iRet = ::recv(pClient->getSocket(), m_pBuffer + m_nRecvedSize, iRecvSize, 0);
	if (iRet == 0)
	{
		EzLogError(_T("client[%x] recv returns 0, the socket is closed.\n"), pClient);
		pClientMgr->OnErrorOccurredNotify(pClient, eSockClosed, 0);
	}
	else if (SOCKET_ERROR == iRet)
	{
		int iErr = ::WSAGetLastError();
		if (WSAEWOULDBLOCK != iErr)
		{
			EzLogError(_T("client[%x] recv fails, error code is %d.\n"), pClient, iErr);
			pClientMgr->OnErrorOccurredNotify(pClient, eSystemError, iErr);
		}
		else
		{
			iRet = ERR_RECV_COMPLETED;
		}
	}

	return iRet;
}
