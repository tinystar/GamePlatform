#include "IocpSendBuffer.h"
#include "IoContext.h"
#include "ClientContext.h"
#include "TcpPackage.h"


bool IocpSendBuffer::init(size_t nMaxPkgSize)
{
	if (!EzVerify(nMaxPkgSize > 0))
		return false;

	m_nMaxPkgSize = nMaxPkgSize;

	m_nBufferSize = calcBufferSize(m_nMaxPkgSize);
	m_pBuffer = new char[m_nBufferSize];

	return m_pBuffer != NULL;
}

bool IocpSendBuffer::asyncSend(ClientContext* pClient, PER_IO_CONTEXT* pIoContext, IClientSessionMgr* pClientMgr, void* pData, size_t nDataLen)
{
	if (!EzVerify(pClient != NULL && pIoContext != NULL && pClientMgr != NULL && pData != NULL))
		return false;

	if (!EzVerify(nDataLen > 0))
		return false;

	if (!EzVerify(m_pBuffer != NULL))
		return false;

	if (nDataLen > m_nMaxPkgSize)
	{
		EzLogError(_T("asyncSend: client[%x] package size is too large.\n"), pClient);
		pClientMgr->OnErrorOccurredNotify(pClient, eInvalidPackage, 0);
		return false;
	}

	TcpPackageHeader header;
	header.uPackageSize = nDataLen;

	size_t nWrite = writeBuffer(&header, sizeof(TcpPackageHeader));
	EzAssert(nWrite == sizeof(TcpPackageHeader));
	if (nWrite == 0)
	{
		EzLogError(_T("the send buffer is maybe not enough!\n"));
		pClientMgr->OnErrorOccurredNotify(pClient, eBufNotEnough, 0);
		return false;
	}

	nWrite = writeBuffer(pData, nDataLen);
	EzAssert(nWrite == nDataLen);
	if (nWrite == 0)
	{
		EzLogError(_T("the send buffer is maybe not enough!\n"));
		pClientMgr->OnErrorOccurredNotify(pClient, eBufNotEnough, 0);
		return false;
	}

	if (!m_bSending)
		return doSend(pClient, pIoContext, pClientMgr);

	return true;
}

size_t IocpSendBuffer::writeBuffer(void* pData, size_t nDataLen)
{
	if (!EzVerify(pData != NULL))
		return 0;

	if (!EzVerify(m_pBuffer != NULL))
		return 0;

	if (m_nUsedSize + nDataLen > m_nBufferSize)
	{
		EzLogError(_T("send buffer is not enough!\n"));
		return 0;
	}

	if (m_nTailPos + nDataLen > m_nBufferSize)
	{
		size_t nTailSize = m_nBufferSize - m_nTailPos;
		::memcpy(m_pBuffer + m_nTailPos, pData, nTailSize);
		::memcpy(m_pBuffer, (EzByte*)pData + nTailSize, nDataLen - nTailSize);
		m_nTailPos = nDataLen - nTailSize;
	}
	else
	{
		::memcpy(m_pBuffer + m_nTailPos, pData, nDataLen);
		m_nTailPos += nDataLen;
		if (m_nTailPos == m_nBufferSize)
			m_nTailPos = 0;
	}

	m_nUsedSize += nDataLen;

	return nDataLen;
}

bool IocpSendBuffer::onSendCompleted(ClientContext* pClient, PER_IO_CONTEXT* pIoContext, size_t nSize, IClientSessionMgr* pClientMgr)
{
	if (!EzVerify(pClient != NULL && pIoContext != NULL && pClientMgr != NULL))
		return false;
	
	m_bSending = false;
	if (EzVerify(nSize > 0 && m_nUsedSize >= nSize))
	{
		m_nHeadPos += nSize;
		if (m_nHeadPos >= m_nBufferSize)
			m_nHeadPos = m_nHeadPos % m_nBufferSize;
		m_nUsedSize -= nSize;

		if (isEmpty())
		{
			EzAssert(m_nHeadPos == m_nTailPos);
			m_nHeadPos = m_nTailPos = 0;
		}
		else
		{
			bool bRet = doSend(pClient, pIoContext, pClientMgr);
		}
		return true;
	}

	return false;
}

size_t IocpSendBuffer::calcBufferSize(size_t nPkgSize)
{
	if (nPkgSize < 1024)
		return 4096;

	return 4 * nPkgSize;
}

bool IocpSendBuffer::doSend(ClientContext* pClient, PER_IO_CONTEXT* pIoContext, IClientSessionMgr* pClientMgr)
{
	if (!EzVerify(pClient != NULL && pIoContext != NULL && pClientMgr != NULL))
		return false;

	if (!EzVerify(m_pBuffer != NULL))
		return false;

	if (!m_bSending && !isEmpty())
	{
		m_bSending = true;

		size_t nSendSize = 0;
		if (m_nTailPos > m_nHeadPos)
			nSendSize = m_nTailPos - m_nHeadPos;
		else
			nSendSize = m_nBufferSize - m_nHeadPos;
		EzAssert(nSendSize != 0);

		pIoContext->wsaBuffer.buf = m_pBuffer + m_nHeadPos;
		pIoContext->wsaBuffer.len = nSendSize;
		pIoContext->type = eOpSend;

		DWORD dwTransferred = 0;
		int iRet = ::WSASend(pClient->getSocket(), &pIoContext->wsaBuffer, 1, &dwTransferred, 0, &pIoContext->overlapped, NULL);
		if (iRet != 0)
		{
			int iErrCode = ::WSAGetLastError();
			if (iErrCode != WSA_IO_PENDING)
			{
				EzTrace(_T("WSASend error!error code = %d\n"), iErrCode);
				EzLogError(_T("WSASend fails in %s, line %d.\n"), _T(__FUNCTION__), __LINE__);
 				pClientMgr->OnErrorOccurredNotify(pClient, eSystemError, iErrCode);
				return false;
			}
		}
	}

	return true;
}
