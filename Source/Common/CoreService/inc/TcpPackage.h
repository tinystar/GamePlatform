/************************************************************************/
/* All rights reserved.
/* You can't use, copy or modify the file without the permission of the
/* author. Otherwise I will be held accountable under the law.
/*
/* Author: xiaoxin
/* Contact: 765392700(QQ)
/* Date: 2017/1/25
/************************************************************************/
#ifndef __TCP_PACKAGE_H__
#define __TCP_PACKAGE_H__

#include "ServiceTypes.h"

#pragma pack(push, 1)

struct TcpPackageHeader
{
	CSUINT32		uPackageSize;		// package body size, not include the size of TcpPackageHeader
};

#pragma pack(pop)

#endif // __TCP_PACKAGE_H__
