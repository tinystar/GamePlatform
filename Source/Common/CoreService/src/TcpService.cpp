#include "TcpService.h"
#ifdef EZ_WINDOWS
#include "win/TcpServiceIocp.h"
#elif defined(EZ_LINUX)
// todo
#endif


#ifdef EZ_WINDOWS

class WinSockLazyInit
{
public:
	WinSockLazyInit()
	{
		WSADATA wsaData;
		int err = ::WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (err != 0)
		{
			throw;
		}
	}
	~WinSockLazyInit()
	{
		::WSACleanup();
	}
};

WinSockLazyInit	_lazySockInit;		// 全局初始化winsock

#endif


//-------------------------------------------------------------------------------
// TcpConfig
//-------------------------------------------------------------------------------
const TcpConfig TcpConfig::kDefault;


//-------------------------------------------------------------------------------
// TcpService
//-------------------------------------------------------------------------------
TcpService::TcpService()
	: m_pClientFilter(NULL)
{

}

TcpService::~TcpService()
{
	m_pClientFilter = NULL;
}

bool TcpService::addEventHandler(ITcpServiceEventHandler* pEventHandler)
{
	if (NULL == pEventHandler)
		return false;

	int idx = m_EventHandlers.find(pEventHandler);
	if (idx < 0)
		m_EventHandlers.append(pEventHandler);
	return true;
}

bool TcpService::removeEventHandler(ITcpServiceEventHandler* pEventHandler)
{
	int idx = m_EventHandlers.find(pEventHandler);
	if (idx < 0)
		return false;

	m_EventHandlers.removeAt(idx);
	return true;
}

bool TcpService::registerClientFilter(TcpServiceClientFilter* pClientFilter)
{
	if (m_pClientFilter != pClientFilter)
		m_pClientFilter = pClientFilter;
	return true;
}


//-------------------------------------------------------------------------------
// create/release tcp service
//-------------------------------------------------------------------------------
TcpService* createTcpService(NetModel model /*= kIocp*/)
{
	TcpService* pService = NULL;
	switch (model)
	{
	case kIocp:
#ifdef EZ_WINDOWS
		pService = new TcpServiceIocp();
#else
		EzAssert(!_T("iocp can only be used in windows!"));
#endif
		break;
	default:
		EzAssert(false);
	}
	return pService;
}

void releaseTcpService(TcpService* pService)
{
	if (pService)
		delete pService;
}
