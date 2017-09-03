#include "GateServer.h"
#include "GateMsgDefs.h"


GateServer::GateServer()
{

}

GateServer::~GateServer()
{

}

bool GateServer::onInit()
{
	return true;
}

bool GateServer::onUninit()
{
	return true;
}

bool GateServer::onStart()
{
	return true;
}

bool GateServer::onStop()
{
	return true;
}

void GateServer::onDefaultMsgHandler(ClientId id, GameMsgHeader* pHeader, void* pData, size_t nDataLen)
{
	switch (pHeader->uMainId)
	{
	case MSG_MAINID_GATE:
		{
			switch (pHeader->uSubId)
			{
			case MSG_SUBID_QUERY_CONFIG:
				break;
			case MSG_SUBID_QUERY_MAINSVR_ADDR:
				break;
			default:
				EzAssert(FALSE);
				break;
			}
		}
		break;
	case MSG_MAINID_MAINSVR_DISTRI:
		break;
	default:
		EzAssert(FALSE);
		break;
	}
}
