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

bool BaseGameServer::onInit(const ServerInitConfig&)
{
	return true;
}

bool BaseGameServer::onUninit()
{
	return true;
}

bool BaseGameServer::onStart()
{
	return true;
}

bool BaseGameServer::onStop()
{
	return true;
}

void BaseGameServer::onTcpClientConnectMsg(ClientId id)
{

}

void BaseGameServer::onTcpPackageRecvMsg(ClientId id, void* pPackage, size_t nSize)
{
	if (!EzVerify(pPackage != NULL && nSize >= sizeof(GameMsgHeader)))
	{
		EzLogWarn(_T("BaseGameServer::onTcpPackageRecvMsg invalid package is received!\n"));
		return;
	}

	if (m_pMsgEncryptor != NULL)
		m_pMsgEncryptor->decrypt((unsigned char*)pPackage, nSize);

	GameMsgHeader* pHeader = (GameMsgHeader*)pPackage;
	CSUINT16 uMainId = pHeader->uMainId;
	CSUINT16 uSubId = pHeader->uSubId;
	CSUINT32 uMsgKey = MAKEMSGKEY(uMainId, uSubId);

	NetMsgMap::iterator iter = m_NetMsgMap.find(uMsgKey);
	if (iter != m_NetMsgMap.end())
	{
		NetMsgHandler pMsgHandler = iter->second;
		(this->*pMsgHandler)(id, pPackage, nSize);
	}
	else
	{
		onDefaultMsgHandler(id, pPackage, nSize);
	}
}

void BaseGameServer::onTcpClientCloseMsg(ClientId id)
{

}

void BaseGameServer::onTimerMsg(EzUInt uTimerId)
{

}

void BaseGameServer::onUserItemMsg(int itemId, void* pData, size_t nSize)
{

}

bool BaseGameServer::sendMsg(ClientId id, void* pData, size_t nDataLen)
{
	if (NULL == pData || 0 == nDataLen)
		return false;

	if (m_pMsgEncryptor != NULL)
		m_pMsgEncryptor->encrypt((unsigned char*)pData, nDataLen);

	sendData(id, pData, nDataLen);
	return true;
}

bool BaseGameServer::sendMsg(ClientId id, CSUINT16 uMainId, CSUINT16 uSubId, CSUINT32 uReserved /*= 0*/)
{
	GameMsgHeader header;
	header.uMainId = uMainId;
	header.uSubId = uSubId;
	header.uReserved = uReserved;

	if (m_pMsgEncryptor != NULL)
		m_pMsgEncryptor->encrypt((unsigned char*)&header, sizeof(header));

	sendData(id, &header, sizeof(header));
	return true;
}

bool BaseGameServer::sendMsgToAll(void* pData, size_t nDataLen)
{
	if (NULL == pData || 0 == nDataLen)
		return false;

	if (m_pMsgEncryptor != NULL)
		m_pMsgEncryptor->encrypt((unsigned char*)pData, nDataLen);

	sendDataToAll(pData, nDataLen);
	return true;
}

bool BaseGameServer::sendMsgToAll(CSUINT16 uMainId, CSUINT16 uSubId, CSUINT32 uReserved /*= 0*/)
{
	GameMsgHeader header;
	header.uMainId = uMainId;
	header.uSubId = uSubId;
	header.uReserved = uReserved;

	if (m_pMsgEncryptor != NULL)
		m_pMsgEncryptor->encrypt((unsigned char*)&header, sizeof(header));

	sendDataToAll(&header, sizeof(header));
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

void BaseGameServer::onDefaultMsgHandler(ClientId id, void* pData, size_t nDataLen)
{

}
