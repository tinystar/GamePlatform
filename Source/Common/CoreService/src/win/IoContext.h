/************************************************************************/
/* All rights reserved.
/* You can't use, copy or modify the file without the permission of the
/* author. Otherwise I will be held accountable under the law.
/*
/* Author: xiaoxin
/* Contact: 765392700(QQ)
/* Date: 2017/1/4
/************************************************************************/
#ifndef __IO_CONTEXT_H__
#define __IO_CONTEXT_H__

#include <WinSock2.h>


enum OpType
{
	eOpNone = 0,
	eOpAccept,
	eOpSend,
	eOpRecv
};

struct PER_IO_CONTEXT
{
	OVERLAPPED		overlapped;
	WSABUF			wsaBuffer;
	OpType			type;

	PER_IO_CONTEXT()
		: type(eOpNone)
	{
		::memset(&overlapped, 0, sizeof(overlapped));
		wsaBuffer.buf = NULL;
		wsaBuffer.len = 0;
	}

	void reset()
	{
		type = eOpNone;
		::memset(&overlapped, 0, sizeof(overlapped));
		wsaBuffer.buf = NULL;
		wsaBuffer.len = 0;
	}
};

#define ACCEPT_BUF_SIZE		100		// at least 2*(sizeof(SOCKADDR_IN) + 16)

// used for AcceptEx
struct PER_ACCEPT_CONTEXT : public PER_IO_CONTEXT
{
	SOCKET			hAcceptSocket;
	char			szAcceptBuffer[ACCEPT_BUF_SIZE];

	PER_ACCEPT_CONTEXT()
		: hAcceptSocket(INVALID_SOCKET)
	{
		type = eOpAccept;
		::memset(szAcceptBuffer, 0, sizeof(szAcceptBuffer));
		wsaBuffer.buf = szAcceptBuffer;
		wsaBuffer.len = ACCEPT_BUF_SIZE;
	}
	~PER_ACCEPT_CONTEXT()
	{
		if (hAcceptSocket != INVALID_SOCKET)
		{
			::closesocket(hAcceptSocket);
			hAcceptSocket = INVALID_SOCKET;
		}
	}

	void reset()
	{
		hAcceptSocket = INVALID_SOCKET;		// don't close, set to INVALID_SOCKET directly.
		type = eOpAccept;
		::memset(&overlapped, 0, sizeof(overlapped));
		::memset(szAcceptBuffer, 0, sizeof(szAcceptBuffer));
		wsaBuffer.buf = szAcceptBuffer;
		wsaBuffer.len = ACCEPT_BUF_SIZE;
	}
};

#endif // __IO_CONTEXT_H__
