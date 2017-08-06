/************************************************************************/
/* All rights reserved.
/* You can't use, copy or modify the file without the permission of the
/* author. Otherwise I will be held accountable under the law.
/*
/* Author: xiaoxin
/* Contact: 765392700(QQ)
/* Date: 2017/8/2
/************************************************************************/
#ifndef __XEZ_ENCRYPTOR_H__
#define __XEZ_ENCRYPTOR_H__

#include "xEzCompile.h"

class IEzEncryptor
{
public:
	virtual bool encrypt(unsigned char* pBuffer, size_t nBufSize) = 0;
	virtual bool decrypt(unsigned char* pBuffer, size_t nBufSize) = 0;
};

class EZ_DLL_SPEC EzXorEncryptor
{
public:
	virtual bool encrypt(unsigned char* pBuffer, size_t nBufSize);
	virtual bool decrypt(unsigned char* pBuffer, size_t nBufSize);
};

// substitution encrypt
class EZ_DLL_SPEC EzSubEncryptor
{
public:
	virtual bool encrypt(unsigned char* pBuffer, size_t nBufSize);
	virtual bool decrypt(unsigned char* pBuffer, size_t nBufSize);
};

#endif // __XEZ_ENCRYPTOR_H__