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
 * 使用生产者消费者模型，将TcpService和TimerService产生的消息放到缓冲队列里面，
 * 让另外一个线程（消费者）来专门处理，并调用接口类相应事件处理函数（onXXXMsg）
 *
 * 作用：主要是为了提高底层处理效率，以免由于上层逻辑耗时降低底层处理能力，顺带
 *	     的作用是由于经过消费者线程的处理，所有的消息函数都在同一个线程里面，减少了
 *       上层逻辑的线程同步
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