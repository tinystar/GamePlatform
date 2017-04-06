/************************************************************************/
/* All rights reserved.
/* You can't use, copy or modify the file without the permission of the
/* author. Otherwise I will be held accountable under the law.
/*
/* Author: xiaoxin
/* Contact: 765392700(QQ)
/* Date: 2017/3/23
/************************************************************************/
#ifndef __SERVER_TEMPLATE_IMP_H__
#define __SERVER_TEMPLATE_IMP_H__

#include "ServerTemplate.h"

/**
 * ʹ��������������ģ�ͣ���TcpService��TimerService��������Ϣ�ŵ�����������棬
 * ������һ���̣߳������ߣ���ר�Ŵ��������ýӿ�����Ӧ�¼���������onXXXMsg��
 *
 * ���ã���Ҫ��Ϊ����ߵײ㴦��Ч�ʣ����������ϲ��߼���ʱ���͵ײ㴦��������˳��
 *	     �����������ھ����������̵߳Ĵ������е���Ϣ��������ͬһ���߳����棬������
 *       �ϲ��߼����߳�ͬ��
 */
class ServerTemplateImp : public ITcpServiceEventHandler, public ITimerServiceEventHandler
{
	friend class ServerTemplate;

protected:
	ServerTemplateImp(ServerTemplate* pApiObj);
	virtual ~ServerTemplateImp();
	
protected:
	virtual void onClientConnected(ClientId id);
	virtual void onPackageReceived(ClientId id, void* pPackage, size_t nSize);
	virtual void onClientClosed(ClientId id);

	virtual void onTimerMessage(EzUInt uTimerId);

protected:
	ServerTemplate*		m_pApiObj;
};

#endif // __SERVER_TEMPLATE_IMP_H__