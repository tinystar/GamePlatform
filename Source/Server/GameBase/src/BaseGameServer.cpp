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
	EzAssert(pPackage != NULL && nSize >= sizeof(CSUINT16)*2);

	if (m_pMsgEncryptor != NULL)
		m_pMsgEncryptor->decrypt((unsigned char*)pPackage, nSize);

	CSUINT16 uMainId = *(CSUINT16*)pPackage;
	CSUINT16 uSubId = *((CSUINT16*)pPackage + 1);
	CSUINT32 uMsgKey = MAKEMSGKEY(uMainId, uSubId);

	void* pMsgData = NULL;
	size_t nMsgSize = 0;
	if (nSize > sizeof(CSUINT16) * 2)
	{
		pMsgData = (CSUINT16*)pPackage + 2;
		nMsgSize = nSize - sizeof(CSUINT16) * 2;
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
	size_t nMsgSize = sizeof(CSUINT16) * 2;
	if (pData != NULL && nDataLen > 0)
		nMsgSize += nDataLen;

	unsigned char* pMsgBuffer = (unsigned char*)_alloca(nMsgSize);
	if (NULL == pMsgBuffer)
		return false;

	*((CSUINT16*)pMsgBuffer) = uMainId;
	*((CSUINT16*)pMsgBuffer + 1) = uSubId;
	if (pData != NULL && nDataLen > 0)
		::memcpy(pMsgBuffer + sizeof(CSUINT16)*2, pData, nDataLen);

	if (m_pMsgEncryptor != NULL)
		m_pMsgEncryptor->encrypt(pMsgBuffer, nMsgSize);

	sendData(id, pMsgBuffer, nMsgSize);
	return true;
}

bool BaseGameServer::sendMsgToAll(CSUINT16 uMainId, CSUINT16 uSubId, void* pData /*= NULL*/, size_t nDataLen /*= 0*/)
{
	size_t nMsgSize = sizeof(CSUINT16) * 2;
	if (pData != NULL && nDataLen > 0)
		nMsgSize += nDataLen;

	unsigned char* pMsgBuffer = (unsigned char*)_alloca(nMsgSize);
	if (NULL == pMsgBuffer)
		return false;

	*((CSUINT16*)pMsgBuffer) = uMainId;
	*((CSUINT16*)pMsgBuffer + 1) = uSubId;
	if (pData != NULL && nDataLen > 0)
		::memcpy(pMsgBuffer + sizeof(CSUINT16) * 2, pData, nDataLen);

	if (m_pMsgEncryptor != NULL)
		m_pMsgEncryptor->encrypt(pMsgBuffer, nMsgSize);

	sendDataToAll(pMsgBuffer, nMsgSize);
	return true;
}

bool BaseGameServer::addMsgMapEntry(CSUINT16 uMainId, CSUINT16 uSubId, NetMsgHandler pHandler)
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

bool BaseGameServer::addMsgMapEntry(const NetMsgMapEntry* const pEntries, size_t nEntryCount)
{
	if (NULL == pEntries || 0 == nEntryCount)
		return false;

	for (size_t i = 0; i < nEntryCount; ++i)
		addMsgMapEntry(pEntries[i].uMainId, pEntries[i].uSubId, pEntries[i].pHandler);

	return true;
}

bool BaseGameServer::removeMsgMapEntry(CSUINT16 uMainId, CSUINT16 uSubId)
{
	CSUINT32 uMsgKey = MAKEMSGKEY(uMainId, uSubId);
	NetMsgMap::iterator iter = m_NetMsgMap.find(uMsgKey);
	if (iter == m_NetMsgMap.end())
		return false;

	m_NetMsgMap.erase(iter);
	return true;
}

bool BaseGameServer::removeMsgMapEntry(const NetMsgMapEntry* const pEntries, size_t nEntryCount)
{
	if (NULL == pEntries || 0 == nEntryCount)
		return false;

	for (size_t i = 0; i < nEntryCount; ++i)
		removeMsgMapEntry(pEntries[i].uMainId, pEntries[i].uSubId);

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
