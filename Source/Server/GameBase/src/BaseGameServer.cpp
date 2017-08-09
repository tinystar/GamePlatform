#include "BaseGameServer.h"

#define MAKEMSGKEY(mainId, subId)		((CSUINT32)( (((CSUINT16)mainId) << 16) | ((CSUINT16)subId) ))


BaseGameServer::BaseGameServer()
	: m_pMsgEncryptor(NULL)
{

}

BaseGameServer::~BaseGameServer()
{
	EZ_SAFE_DELETE(m_pMsgEncryptor);
}

bool BaseGameServer::onInit()
{
	return false;
}

bool BaseGameServer::onUninit()
{
	return false;
}

bool BaseGameServer::onStart()
{
	return false;
}

bool BaseGameServer::onStop()
{
	return false;
}

void BaseGameServer::onTcpClientConnectMsg(ClientId id)
{

}

void BaseGameServer::onTcpPackageRecvMsg(ClientId id, void* pPackage, size_t nSize)
{
	if (!EzVerify(pPackage != NULL && nSize >= sizeof(BaseMsgHeader)))
	{
		EzLogWarn(_T("BaseGameServer::onTcpPackageRecvMsg invalid package is received!\n"));
		return;
	}

	if (m_pMsgEncryptor != NULL)
		m_pMsgEncryptor->decrypt((unsigned char*)pPackage, nSize);

	BaseMsgHeader* pHeader = (BaseMsgHeader*)pPackage;
	CSUINT16 uMainId = pHeader->uMainId;
	CSUINT16 uSubId = pHeader->uSubId;
	CSUINT32 uMsgKey = MAKEMSGKEY(uMainId, uSubId);

	void* pMsgData = NULL;
	size_t nMsgSize = 0;
	if (nSize > sizeof(BaseMsgHeader))
	{
		pMsgData = pHeader + 1;
		nMsgSize = nSize - sizeof(BaseMsgHeader);
	}

	NetMsgMap::iterator iter = m_NetMsgMap.find(uMsgKey);
	if (iter != m_NetMsgMap.end())
	{
		NetMsgHandler pMsgHandler = iter->second;
		(this->*pMsgHandler)(id, pMsgData, nMsgSize);
	}
	else
	{
		onDefaultMsgHandler(id, uMainId, uSubId, pMsgData, nMsgSize);
	}
}

void BaseGameServer::onTcpClientCloseMsg(ClientId id)
{

}

void BaseGameServer::onTimerMsg(EzUInt uTimerId)
{

}

bool BaseGameServer::sendMsg(ClientId id, CSUINT16 uMainId, CSUINT16 uSubId, void* pData /*= NULL*/, size_t nDataLen /*= 0*/)
{
	size_t nMsgSize = sizeof(BaseMsgHeader);
	if (pData != NULL && nDataLen > 0)
		nMsgSize += nDataLen;

	unsigned char* pMsgBuffer = (unsigned char*)_alloca(nMsgSize);
	if (NULL == pMsgBuffer)
		return false;

	BaseMsgHeader* pHeader = (BaseMsgHeader*)pMsgBuffer;
	pHeader->uMainId = uMainId;
	pHeader->uSubId = uSubId;
	pHeader->uReserved = 0;
	if (pData != NULL && nDataLen > 0)
		::memcpy(pHeader + 1, pData, nDataLen);

	if (m_pMsgEncryptor != NULL)
		m_pMsgEncryptor->encrypt(pMsgBuffer, nMsgSize);

	sendData(id, pMsgBuffer, nMsgSize);
	return true;
}

bool BaseGameServer::sendMsgToAll(CSUINT16 uMainId, CSUINT16 uSubId, void* pData /*= NULL*/, size_t nDataLen /*= 0*/)
{
	size_t nMsgSize = sizeof(BaseMsgHeader);
	if (pData != NULL && nDataLen > 0)
		nMsgSize += nDataLen;

	unsigned char* pMsgBuffer = (unsigned char*)_alloca(nMsgSize);
	if (NULL == pMsgBuffer)
		return false;

	BaseMsgHeader* pHeader = (BaseMsgHeader*)pMsgBuffer;
	pHeader->uMainId = uMainId;
	pHeader->uSubId = uSubId;
	pHeader->uReserved = 0;
	if (pData != NULL && nDataLen > 0)
		::memcpy(pHeader + 1, pData, nDataLen);

	if (m_pMsgEncryptor != NULL)
		m_pMsgEncryptor->encrypt(pMsgBuffer, nMsgSize);

	sendDataToAll(pMsgBuffer, nMsgSize);
	return true;
}

bool BaseGameServer::registerMsgHandler(CSUINT16 uMainId, CSUINT16 uSubId, NetMsgHandler pHandler)
{
	if (!EzVerify(pHandler != NULL))
		return false;

	CSUINT32 uMsgKey = MAKEMSGKEY(uMainId, uSubId);
	NetMsgMap::iterator iter = m_NetMsgMap.find(uMsgKey);
	if (iter != m_NetMsgMap.end())
	{
		EzAssert(!_T("the msg is already in the map!"));
		return false;
	}

	m_NetMsgMap.insert(std::make_pair(uMsgKey, pHandler));
	return true;
}

bool BaseGameServer::registerMsgHandler(const NetMsgMapEntry* const pEntries, size_t nEntryCount)
{
	if (NULL == pEntries || 0 == nEntryCount)
		return false;

	for (size_t i = 0; i < nEntryCount; ++i)
		registerMsgHandler(pEntries[i].uMainId, pEntries[i].uSubId, pEntries[i].pHandler);

	return true;
}

bool BaseGameServer::removeMsgHandler(CSUINT16 uMainId, CSUINT16 uSubId)
{
	CSUINT32 uMsgKey = MAKEMSGKEY(uMainId, uSubId);
	NetMsgMap::iterator iter = m_NetMsgMap.find(uMsgKey);
	if (iter == m_NetMsgMap.end())
		return false;

	m_NetMsgMap.erase(iter);
	return true;
}

bool BaseGameServer::removeMsgHandler(const NetMsgMapEntry* const pEntries, size_t nEntryCount)
{
	if (NULL == pEntries || 0 == nEntryCount)
		return false;

	for (size_t i = 0; i < nEntryCount; ++i)
		removeMsgHandler(pEntries[i].uMainId, pEntries[i].uSubId);

	return true;
}

void BaseGameServer::setMsgEncryptor(IEzEncryptor* pEncryptor)
{
	if (m_pMsgEncryptor != pEncryptor)
	{
		EZ_SAFE_DELETE(m_pMsgEncryptor);
		m_pMsgEncryptor = pEncryptor;
	}
}

void BaseGameServer::onDefaultMsgHandler(ClientId id, CSUINT16 uMainId, CSUINT16 uSubId, void* pData, size_t nDataLen)
{

}
