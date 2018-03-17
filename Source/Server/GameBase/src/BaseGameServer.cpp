#include "BaseGameServer.h"
#include "BaseMsgDefs.h"

#define MAKEMSGKEY(mainId, subId)		((CSUINT32)( (((CSUINT16)mainId) << 16) | ((CSUINT16)subId) ))


const NetMsgTable* BaseGameServer::GetNetMsgTable() const
{
	return GetThisNetMsgTable();
}

const NetMsgTable* BaseGameServer::GetThisNetMsgTable()
{
	static const NetMsgEntry _messageEntries[] =
	{
		{ 0, 0, (NetMsgHandler)0 }
	};

	static const NetMsgTable messageTable =
	{
		NULL,
		&_messageEntries[0]
	};
	return &messageTable;
}

BaseGameServer::BaseGameServer()
	: m_pMsgEncryptor(NULL)
{

}

BaseGameServer::~BaseGameServer()
{
	EZ_SAFE_DELETE(m_pMsgEncryptor);
}

bool BaseGameServer::onInit(const ServerInitConfig& serverConfig)
{
	buildNetMsgMap();
	return ServerTemplate::onInit(serverConfig);
}

bool BaseGameServer::onUninit()
{
	clearNetMsgMap();
	return ServerTemplate::onUninit();
}

bool BaseGameServer::onStart()
{
	return ServerTemplate::onStart();
}

bool BaseGameServer::onStop()
{
	return ServerTemplate::onStop();
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
	m_NetMsgMap[uMsgKey] = pHandler;
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

void BaseGameServer::buildNetMsgMap()
{
	registerMsgHandler(GetNetMsgTable());
}

void BaseGameServer::clearNetMsgMap()
{
	m_NetMsgMap.clear();
}

// 采用递归方式进行注册，保证基类消息映射表先注册，这样派生类相同的消息就会覆盖基类的消息，
// 以达到类似多态的效果
void BaseGameServer::registerMsgHandler(const NetMsgTable* pMsgTable)
{
	if (NULL == pMsgTable)
		return;

	if (pMsgTable->pfnGetBaseTable != NULL)
		registerMsgHandler(pMsgTable->pfnGetBaseTable());

	const NetMsgEntry* pEntry = pMsgTable->pEntries;
	while (pEntry->uMainId != 0 || pEntry->uSubId != 0 || pEntry->pHandler != NULL)
	{
		registerMsgHandler(pEntry->uMainId, pEntry->uSubId, pEntry->pHandler);
		++pEntry;
	}
}
