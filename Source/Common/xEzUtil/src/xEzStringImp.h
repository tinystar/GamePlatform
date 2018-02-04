/************************************************************************/
/* All rights reserved.
/* You can't use, copy or modify the file without the permission of the
/* author. Otherwise I will be held accountable under the law.
/*
/* Author: xiaoxin
/* Contact: 765392700(QQ)
/* Date: 2018/1/21
/************************************************************************/
#ifndef __XEZ_STRING_IMP_H__
#define __XEZ_STRING_IMP_H__

#include "xEzString.h"
#include "xEzSmartPtr.h"
#include "xEzTextConvertor.h"
#include <malloc.h>
#include <windows.h>

template<typename T>
class StringOper;

template<>
class StringOper < char >
{
public:
	static size_t strlen(const char* pString)
	{
		return ::strlen(pString);
	}

	static char* strcpy(char* pDst, const char* pSrc)
	{
		return ::strcpy(pDst, pSrc);
	}

	static char* strncpy(char* pDst, const char* pSrc, size_t count)
	{
		return ::strncpy(pDst, pSrc, count);
	}

	static int strcmp(const char* pStr1, const char* pStr2)
	{
		return ::strcmp(pStr1, pStr2);
	}

	static int stricmp(const char* pStr1, const char* pStr2)
	{
		return ::_stricmp(pStr1, pStr2);
	}

	static char* strcat(char* pDst, const char* pSrc)
	{
		return ::strcat(pDst, pSrc);
	}

	static char* strncat(char* pDst, const char* pSrc, size_t count)
	{
		return ::strncat(pDst, pSrc, count);
	}
};

template<>
class StringOper < wchar_t >
{
public:
	static size_t strlen(const wchar_t* pString)
	{
		return ::wcslen(pString);
	}

	static wchar_t* strcpy(wchar_t* pDst, const wchar_t* pSrc)
	{
		return ::wcscpy(pDst, pSrc);
	}

	static wchar_t* strncpy(wchar_t* pDst, const wchar_t* pSrc, size_t count)
	{
		return ::wcsncpy(pDst, pSrc, count);
	}

	static int strcmp(const wchar_t* pStr1, const wchar_t* pStr2)
	{
		return ::wcscmp(pStr1, pStr2);
	}

	static int stricmp(const wchar_t* pStr1, const wchar_t* pStr2)
	{
		return ::_wcsicmp(pStr1, pStr2);
	}

	static wchar_t* strcat(wchar_t* pDst, const wchar_t* pSrc)
	{
		return ::wcscat(pDst, pSrc);
	}

	static wchar_t* strncat(wchar_t* pDst, const wchar_t* pSrc, size_t count)
	{
		return ::wcsncat(pDst, pSrc, count);
	}
};

template< typename Ch, typename StrOp = StringOper<Ch> >
class StringBuffer
{
public:
	StringBuffer()
		: m_pBuffer(_s_strEmpty)
		, m_uDataLen(0)
		, m_uAllocLen(0)
	{}

	StringBuffer(const StringBuffer& other)
		: m_pBuffer(_s_strEmpty)
		, m_uDataLen(0)
		, m_uAllocLen(0)
	{
		copyStrData(other.m_pBuffer, other.m_uDataLen);
	}

	StringBuffer(const Ch* pStrData, int nStrLen = -1)
		: m_pBuffer(_s_strEmpty)
		, m_uDataLen(0)
		, m_uAllocLen(0)
	{
		copyStrData(pStrData, nStrLen);
	}

	~StringBuffer()
	{
		reset();
	}

public:
	StringBuffer& operator= (const StringBuffer& other)
	{
		if (this != &other)
			copyStrData(other.m_pBuffer, other.m_uDataLen);

		return *this;
	}

	StringBuffer& operator= (const Ch* pOther)
	{
		if (m_pBuffer != pOther)
			copyStrData(pOther);

		return *this;
	}

	operator const Ch*() const
	{
		return buffer();
	}

	friend bool operator== (const StringBuffer& str1, const StringBuffer& str2)
	{
		return (0 == str1.compare(str2));
	}

	friend bool operator== (const StringBuffer& str1, const Ch* psz2)
	{
		return (0 == str1.compare(psz2));
	}

	friend bool operator== (const Ch* psz1, const StringBuffer& str2)
	{
		return (0 == str2.compare(psz1));
	}

	StringBuffer& operator+= (const StringBuffer& other)
	{
		concat(other.m_pBuffer, other.m_uDataLen);
		return *this;
	}

	StringBuffer& operator+= (const Ch* pszOther)
	{
		concat(pszOther);
		return *this;
	}

	friend StringBuffer operator+ (const StringBuffer& str1, const StringBuffer& str2)
	{
		StringBuffer result = str1;
		result += str2;
		return result;
	}

	friend StringBuffer operator+ (const StringBuffer& str1, const Ch* psz2)
	{
		StringBuffer result = str1;
		result += psz2;
		return result;
	}

	friend StringBuffer operator+ (const Ch* psz1, const StringBuffer& str2)
	{
		StringBuffer result = str2;
		result += psz1;
		return result;
	}

public:
	unsigned int dataLength() const { return m_uDataLen; }
	unsigned int allocLength() const { return m_uAllocLen; }

	const Ch* buffer() const { return m_pBuffer; }

	bool copyStrData(Ch c, unsigned int uCount)
	{
		if (allocBuffer(uCount))
		{
			for (unsigned int i = 0; i < uCount; ++i)
				m_pBuffer[i] = c;
			m_uDataLen = uCount;
			m_pBuffer[m_uDataLen] = 0;
			return true;
		}

		return false;
	}

	bool copyStrData(const Ch* pStrData, int nStrLen = -1)
	{
		if (nStrLen < 0)
			nStrLen = (int)StrOp::strlen(pStrData);

		if (allocBuffer(nStrLen))
		{
			StrOp::strncpy(m_pBuffer, pStrData, nStrLen);
			m_uDataLen = nStrLen;
			m_pBuffer[m_uDataLen] = 0;
			return true;
		}

		return false;
	}

	int compare(const Ch* pszOther) const
	{
		if (NULL == pszOther)
			pszOther = _s_strEmpty;

		return StrOp::strcmp(m_pBuffer, pszOther);
	}

	int compareNoCase(const Ch* pszOther) const
	{
		if (NULL == pszOther)
			pszOther = _s_strEmpty;

		return StrOp::stricmp(m_pBuffer, pszOther);
	}

	void concat(const Ch* pszStr, int nStrLen = -1)
	{
		if (nStrLen < 0)
			nStrLen = (int)StrOp::strlen(pszStr);

		Ch* pOldBuffer = NULL;
		if (m_uDataLen + nStrLen > m_uAllocLen)
		{
			pOldBuffer = m_pBuffer;
			m_pBuffer = (Ch*)malloc((m_uDataLen + nStrLen + 1) * sizeof(Ch));
			if (!EzVerify(m_pBuffer != NULL))
			{
				freeBuffer(pOldBuffer);
				m_uAllocLen = m_uDataLen = 0;
				return;
			}

			m_uAllocLen = m_uDataLen + nStrLen;
			if (m_uDataLen > 0)
				StrOp::strncpy(m_pBuffer, pOldBuffer, m_uDataLen);
			m_pBuffer[m_uDataLen] = 0;
		}

		StrOp::strncat(m_pBuffer, pszStr, nStrLen);
		if (pOldBuffer != NULL)		// Free the old buffer after concat because it may be concat with itself, that is pszStr is equal to pOldBuffer
			freeBuffer(pOldBuffer);
		m_uDataLen += nStrLen;
		m_pBuffer[m_uDataLen] = 0;
	}

	void reset()
	{
		freeBuffer(m_pBuffer);
		m_pBuffer = _s_strEmpty;

		m_uAllocLen = m_uDataLen = 0;
	}

protected:
	bool allocBuffer(unsigned int uChars)
	{
		if (0 == uChars)
			return true;
		if (uChars <= m_uAllocLen)
			return true;

		reset();
		m_pBuffer = (Ch*)malloc((uChars + 1) * sizeof(Ch));
		if (!EzVerify(m_pBuffer != NULL))
		{
			m_uAllocLen = m_uDataLen = 0;
			return false;
		}

		m_pBuffer[uChars] = 0;
		m_uAllocLen = uChars;
		return true;
	}

	void freeBuffer(Ch* pBuffer)
	{
		if (pBuffer != _s_strEmpty)
			EZ_SAFE_FREE(pBuffer);
	}

private:
	static Ch	 _s_strEmpty[1];

	Ch*			 m_pBuffer;
	unsigned int m_uDataLen;		// not including terminating null
	unsigned int m_uAllocLen;		// not including terminating null
};

template<typename Ch, typename StrOp> Ch StringBuffer<Ch, StrOp>::_s_strEmpty[1] = { 0 };


class StringData
{
	friend EzString;

	enum SyncDirection
	{
		kSyncNotNeeded = 0,
		kSyncFromStr = 1,
		kSyncFromWStr = 2
	};

public:
	void addRef()
	{
		++m_nRefs;
	}

	void release()
	{
		--m_nRefs;
		if (0 == m_nRefs)
			delete this;
	}

	const EzRefCounter& refCount() const { return m_nRefs; }

	void copyString(char ch, unsigned int uCount)
	{
		if (m_chBuffer.copyStrData(ch, uCount))
		{
			m_syncDir = kSyncFromStr;
			m_chEncoding = kAnsi;
		}
	}

	void copyString(wchar_t wch, unsigned int uCount)
	{
		if (m_wchBuffer.copyStrData(wch, uCount))
			m_syncDir = kSyncFromWStr;
	}

	void copyString(const char* pszString, TextEncoding encoding)
	{
		if (encoding != kUnknown && m_chBuffer.copyStrData(pszString))
		{
			m_syncDir = kSyncFromStr;
			m_chEncoding = encoding;
		}
	}

	void copyString(const wchar_t* pszString)
	{
		if (m_wchBuffer.copyStrData(pszString))
			m_syncDir = kSyncFromWStr;
	}

	const wchar_t* c_wstr() const
	{
		if (kSyncFromStr == m_syncDir)
			const_cast<StringData*>(this)->syncFromString();

		const wchar_t* pStr = m_wchBuffer.buffer();
		return (NULL == pStr) ? L"" : pStr;
	}

	const char* c_str(TextEncoding encoding) const
	{
		if (kSyncFromWStr == m_syncDir)
			const_cast<StringData*>(this)->syncFromWString(encoding);
		else if (m_chEncoding != encoding)
			const_cast<StringData*>(this)->convertEncodingTo(encoding);

		const char* pStr = m_chBuffer.buffer();
		return (NULL == pStr) ? "" : pStr;
	}

	void concat(const StringData* pOther)
	{
		if (NULL == pOther)
			return;

		if (kSyncNotNeeded == pOther->m_syncDir ||
			kSyncFromWStr == pOther->m_syncDir)
		{
			concat(pOther->m_wchBuffer.buffer());
			return;
		}

		if (kUnknown == pOther->m_chEncoding)
			return;

		if (kSyncFromWStr == m_syncDir)
			syncFromWString(m_chEncoding);

		if (m_chEncoding == pOther->m_chEncoding)
		{
			m_chBuffer += pOther->m_chBuffer;
			m_syncDir = kSyncFromStr;
		}
		else
		{
			const char* pszConvert = NULL;
			if (kAnsi == pOther->m_chEncoding)
				pszConvert = EzText::ansiToUtf8(pOther->m_chBuffer.buffer());
			else if (kUtf8 == pOther->m_chEncoding)
				pszConvert = EzText::utf8ToAnsi(pOther->m_chBuffer.buffer());

			if (!EzVerify(pszConvert != NULL))
				return;

			m_chBuffer += pszConvert;
			::free((void*)pszConvert);
			m_syncDir = kSyncFromStr;
		}
	}

	void concat(const wchar_t* pwszOther)
	{
		if (NULL == pwszOther || 0 == *pwszOther)
			return;

		if (kSyncFromStr == m_syncDir)
			syncFromString();

		m_wchBuffer += pwszOther;
		m_syncDir = kSyncFromWStr;
	}

	bool syncFromString()
	{
		if (m_syncDir != kSyncFromStr)
			return false;
		if (m_chEncoding == kUnknown)
			return false;

		const wchar_t* pszWStr = NULL;
		if (kAnsi == m_chEncoding)
			pszWStr = EzText::ansiToWideChar(m_chBuffer);
		else if (kUtf8 == m_chEncoding)
			pszWStr = EzText::utf8ToWideChar(m_chBuffer);

		if (!EzVerify(pszWStr != NULL))
			return false;

		m_wchBuffer = pszWStr;
		::free((void*)pszWStr);
		m_syncDir = kSyncNotNeeded;
		return true;
	}

	bool syncFromWString(TextEncoding encoding)
	{
		if (m_syncDir != kSyncFromWStr)
			return false;
		if (encoding != kUnknown && m_chEncoding != encoding)
			m_chEncoding = encoding;

		if (kUnknown == m_chEncoding)
			return false;

		const char* pszStr = NULL;
		if (kAnsi == m_chEncoding)
			pszStr = EzText::wideCharToAnsi(m_wchBuffer);
		else if (kUtf8 == m_chEncoding)
			pszStr = EzText::wideCharToUtf8(m_wchBuffer);

		if (!EzVerify(pszStr != NULL))
			return false;

		m_chBuffer = pszStr;
		::free((void*)pszStr);
		m_syncDir = kSyncNotNeeded;
		return true;
	}

	void convertEncodingTo(TextEncoding encoding)
	{
		if (m_chEncoding == encoding)
			return;
		if (kUnknown == encoding)
			return;

		const char* psz = m_chBuffer.buffer();
		if (NULL == psz || 0 == *psz)
			return;

		const char* pConvertTo = NULL;
		if (kUtf8 == encoding)
			pConvertTo = EzText::ansiToUtf8(psz);
		else if (kAnsi == encoding)
			pConvertTo = EzText::utf8ToAnsi(psz);

		if (!EzVerify(pConvertTo != NULL))
			return;

		m_chBuffer = pConvertTo;
		::free((void*)pConvertTo);
		m_chEncoding = encoding;
	}

	StringData* clone() const
	{
		StringData* pClone = new StringData();
		if (!EzVerify(pClone != NULL))
			return NULL;

		// only clone data not including reference count
		pClone->m_syncDir = m_syncDir;
		pClone->m_chEncoding = m_chEncoding;
		pClone->m_chBuffer = m_chBuffer;
		pClone->m_wchBuffer = m_wchBuffer;
		return pClone;
	}

private:
	StringData()
		: m_nRefs(1)
		, m_syncDir(kSyncNotNeeded)
		, m_chEncoding(kUnknown)
	{}

	// none copyable, no implementation
	StringData(const StringData&);
	StringData& operator= (const StringData&);

private:
	EzRefCounter			m_nRefs;
	SyncDirection			m_syncDir;
	TextEncoding			m_chEncoding;
	StringBuffer<char>		m_chBuffer;
	StringBuffer<wchar_t>	m_wchBuffer;
};

#endif // __XEZ_STRING_IMP_H__