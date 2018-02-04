/************************************************************************/
/* All rights reserved.
/* You can't use, copy or modify the file without the permission of the
/* author. Otherwise I will be held accountable under the law.
/*
/* Author: xiaoxin
/* Contact: 765392700(QQ)
/* Date: 2018/1/21
/************************************************************************/
#ifndef __XEZ_STRING_H__
#define __XEZ_STRING_H__

#include "xEzCompile.h"

enum TextEncoding
{
	kUnknown	= 0,
	kAnsi		= 1,
	kUtf8		= 2			// no BOM
};

class StringData;

class EZ_DLL_SPEC EzString
{
public:
	EzString();
	explicit EzString(char ch, unsigned int uCount = 1);
	explicit EzString(wchar_t wch, unsigned int uCount = 1);
	EzString(const char* pszString, TextEncoding encoding);
	EzString(const wchar_t* pszString);
	EzString(const EzString& other);

	~EzString();

public:
	// encoding maybe change
	const char* kcharPtr(TextEncoding encoding) const;
	const wchar_t* kwcharPtr() const;

	EzString& operator= (const EzString& other);
	EzString& operator= (const wchar_t* pwszOther);
	EzString& operator+= (const EzString& other);
	EzString& operator+= (const wchar_t* pwszOther);
	friend EzString operator+ (const EzString& str1, const EzString& str2);

	friend bool operator== (const EzString& str1, const EzString& str2)
	{
		return (0 == str1.compare(str2));
	}

	friend bool operator== (const EzString& str1, const wchar_t* psz2)
	{
		return (0 == str1.compare(psz2));
	}

	friend bool operator== (const wchar_t* psz1, const EzString& str2)
	{
		return (0 == str2.compare(psz1));
	}

	TextEncoding getEncoding() const;

	// length of the character size.
	unsigned int chLength() const;
	// length of the function strlen return
	unsigned int strLength() const;
	unsigned int wstrLength() const;

	int compare(const EzString& other) const;
	int compareNoCase(const EzString& other) const;
	int compare(const wchar_t* pszOther) const;
	int compareNoCase(const wchar_t* pszOther) const;

private:
	void copyOnWrite();

private:
	StringData*	m_pStrData;
};

inline EzString operator+ (const EzString& str1, const EzString& str2)
{
	EzString ret = str1;
	ret += str2;
	return ret;
}

#endif // __XEZ_STRING_H__