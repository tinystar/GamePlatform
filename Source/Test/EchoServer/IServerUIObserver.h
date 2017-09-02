#ifndef __ISERVER_UIOBSERVER_H__
#define __ISERVER_UIOBSERVER_H__

class IServerUIObserver
{
public:
	virtual void onClientConnect(ClientId id) = 0;
	virtual void onClientClose(ClientId id) = 0;
};

#endif // __ISERVER_UIOBSERVER_H__