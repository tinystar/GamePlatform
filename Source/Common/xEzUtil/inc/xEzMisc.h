/************************************************************************/
/* All rights reserved.
/* You can't use, copy or modify the file without the permission of the
/* author. Otherwise I will be held accountable under the law.
/*
/* Author: xiaoxin
/* Contact: 765392700(QQ)
/* Date: 2018/1/28
/************************************************************************/
#ifndef __XEZ_MISC_H__
#define __XEZ_MISC_H__


template<typename T>
void EzSwap(T& a, T& b)
{
	T tmp = a;
	a = b;
	b = tmp;
}

#endif // __XEZ_MISC_H__