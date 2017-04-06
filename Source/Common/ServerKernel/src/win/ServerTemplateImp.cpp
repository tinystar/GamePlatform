#include "ServerTemplateImp.h"


ServerTemplateImp::ServerTemplateImp(ServerTemplate* pApiObj)
	: m_pApiObj(pApiObj)
{
}

ServerTemplateImp::~ServerTemplateImp()
{
}

void ServerTemplateImp::onClientConnected(ClientId id)
{

}

void ServerTemplateImp::onPackageReceived(ClientId id, void* pPackage, size_t nSize)
{

}

void ServerTemplateImp::onClientClosed(ClientId id)
{

}

void ServerTemplateImp::onTimerMessage(EzUInt uTimerId)
{

}
