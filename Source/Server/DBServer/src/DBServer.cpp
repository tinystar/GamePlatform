#include "DBServer.h"
#include "DBMsgDefs.h"


DBServer::DBServer()
{

}

DBServer::~DBServer()
{

}

bool DBServer::onInit(const ServerInitConfig&)
{
	return true;
}

bool DBServer::onUninit()
{
	return true;
}

bool DBServer::onStart()
{
	return true;
}

bool DBServer::onStop()
{
	return true;
}
