#include "GameModuleDef.h"
#include "ServerGameDesk.h"


extern "C" bool GameAppEntryPoint(AppMsgCode msg)
{
	switch (msg)
	{
	case kInitAppMsg:
		break;
	case kUnloadAppMsg:
		break;
	default:
		break;
	}

	return true;
}

extern "C" const char* GetGameAppVersion(void)
{
	return "1.0.0";
}

extern "C" GameDeskFactory* GetGameDeskFactory(void)
{
	static GameDeskFactoryTemplate<ServerGameDesk> _s_factory;
	return &_s_factory;
}
