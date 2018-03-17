#include "ServerTemplate.h"
#ifdef EZ_WINDOWS
#include "win\ServerTemplateImp.h"
#elif defined(EZ_LINUX)
// Todo
#endif

enum
{
	kInited		= 0x00000001,
	kRunning	= 0x00000002
};


ServerTemplate::ServerTemplate()
	: m_pServerImp(NULL)
	, m_pTcpService(NULL)
	, m_pTimerService(NULL)
	, m_state(0)
{
	m_pServerImp = new ServerTemplateImp(this);
	if (!EzVerify(m_pServerImp != NULL))
	{
		EzLogFatal(_T("create ServerTemplateImp failed, maybe memory is not enough!\n"));
		throw;
	}
}

ServerTemplate::~ServerTemplate()
{
	stop();
	unInit();

	EZ_SAFE_DELETE(m_pServerImp);
}

SVCErrorCode ServerTemplate::init(const ServerInitConfig& serverConfig)
{
	if (m_state & kInited)
		return eOk;

	SVCErrorCode ec = eOk;
	if (serverConfig.uCreateFlags | kCreateTcpService)
	{
		m_pTcpService = createTcpService();
		if (!EzVerify(m_pTcpService != NULL))
		{
			EzLogFatal(_T("create tcp service failed, maybe memory is not enough!\n"));
			return eMemoryNotEnough;
		}

		ec = m_pTcpService->init(serverConfig.tcpConfig);
		if (!EzVerify(ec == eOk))
		{
			::releaseTcpService(m_pTcpService);
			m_pTcpService = NULL;
			return ec;
		}

		EzVerify(m_pTcpService->addEventHandler(m_pServerImp));
	}

	if (serverConfig.uCreateFlags | kCreateTimerService)
	{
		m_pTimerService = createTimerService();
		if (!EzVerify(m_pTimerService != NULL))
		{
			EzLogFatal(_T("create timer service failed, maybe memory is not enough!\n"));
			if (m_pTcpService != NULL)
			{
				::releaseTcpService(m_pTcpService);
				m_pTcpService = NULL;
			}
			return eMemoryNotEnough;
		}

		ec = m_pTimerService->init();
		if (!EzVerify(ec == eOk))
		{
			if (m_pTcpService != NULL)
			{
				::releaseTcpService(m_pTcpService);
				m_pTcpService = NULL;
			}
			::releaseTimerService(m_pTimerService);
			m_pTimerService = NULL;

			return ec;
		}

		EzVerify(m_pTimerService->addEventHandler(m_pServerImp));
	}

	m_state |= kInited;

	if (!onInit(serverConfig))
	{
		unInit();
		return eNotApplicable;
	}

	return eOk;
}

SVCErrorCode ServerTemplate::unInit()
{
	if (!(m_state & kInited))
		return eOk;

	onUninit();

	if (m_pTcpService != NULL)
	{
		m_pTcpService->removeEventHandler(m_pServerImp);
		m_pTcpService->unInit();
		::releaseTcpService(m_pTcpService);
		m_pTcpService = NULL;
	}

	if (m_pTimerService != NULL)
	{
		m_pTimerService->removeEventHandler(m_pServerImp);
		m_pTimerService->unInit();
		::releaseTimerService(m_pTimerService);
		m_pTimerService = NULL;
	}

	m_state &= ~kInited;
	return eOk;
}

SVCErrorCode ServerTemplate::start()
{
	if (!(m_state & kInited))
		return eNotInitialized;
	if (m_state & kRunning)
		return eOk;

	if (!onStart())
		return eNotApplicable;

	if (!m_pServerImp->start())
		return eSystemError;

	SVCErrorCode ec = eOk;
	if (m_pTcpService != NULL)
		ec = m_pTcpService->start();

	if (ec != eOk)
		return ec;

	if (m_pTimerService != NULL)
		ec = m_pTimerService->start();

	if (eOk == ec)
		m_state |= kRunning;

	return ec;
}

SVCErrorCode ServerTemplate::stop()
{
	if (!(m_state & kRunning))
		return eOk;

	if (m_pTcpService != NULL)
		m_pTcpService->stop();

	if (m_pTimerService != NULL)
		m_pTimerService->stop();

	m_pServerImp->stop();

	onStop();

	m_state &= ~kRunning;
	return eOk;
}

bool ServerTemplate::setTimer(EzUInt uTimerId, EzUInt uElapse)
{
	if (m_pTimerService)
		return m_pTimerService->setTimer(uTimerId, uElapse);
	
	return false;
}

bool ServerTemplate::killTimer(EzUInt uTimerId)
{
	if (m_pTimerService)
		return m_pTimerService->killTimer(uTimerId);

	return false;
}

bool ServerTemplate::sendData(ClientId id, void* pData, size_t nDataLen)
{
	if (m_pTcpService)
		return m_pTcpService->sendData(id, pData, nDataLen);

	return false;
}

bool ServerTemplate::sendDataToAll(void* pData, size_t nDataLen)
{
	if (m_pTcpService)
		return m_pTcpService->sendDataToAll(pData, nDataLen);

	return false;
}

TcpService* ServerTemplate::createTcpService() const
{
#ifdef EZ_WINDOWS
	return ::createTcpService(kIocp);
#elif defined(EZ_LINUX)
	return NULL;
#endif
}

TimerService* ServerTemplate::createTimerService() const
{
	return ::createTimerService();
}

bool ServerTemplate::queueUserItem(int itemId, void* pData, size_t nSize)
{
	return m_pServerImp->queueUserItem(itemId, pData, nSize);
}
