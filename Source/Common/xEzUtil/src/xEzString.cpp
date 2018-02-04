#include "xEzString.h"
#include "xEzStringImp.h"
#include <mbstring.h>


EzString::EzString()
	: m_pStrData(NULL)
{

}

EzString::EzString(char ch, unsigned int uCount /*= 1*/)
	: m_pStrData(NULL)
{
	if (uCount > 0)
	{
		m_pStrData = new StringData();
		EzAssert(m_pStrData != NULL);

		m_pStrData->copyString(ch, uCount);
	}
}

EzString::EzString(wchar_t wch, unsigned int uCount /*= 1*/)
	: m_pStrData(NULL)
{
	if (uCount > 0)
	{
		m_pStrData = new StringData();
		EzAssert(m_pStrData != NULL);

		m_pStrData->copyString(wch, uCount);
	}
}

EzString::EzString(const char* pszString, TextEncoding encoding)
	: m_pStrData(NULL)
{
	if (pszString != NULL && *pszString != 0 && encoding != kUnknown)
	{
		m_pStrData = new StringData();
		EzAssert(m_pStrData != NULL);

		m_pStrData->copyString(pszString, encoding);
	}
}

EzString::EzString(const wchar_t* pszString)
	: m_pStrData(NULL)
{
	if (pszString != NULL && *pszString != 0)
	{
		m_pStrData = new StringData();
		EzAssert(m_pStrData != NULL);

		m_pStrData->copyString(pszString);
	}
}

EzString::EzString(const EzString& other)
	: m_pStrData(other.m_pStrData)
{
	if (m_pStrData != NULL)
		m_pStrData->addRef();
}

EzString::~EzString()
{
	if (m_pStrData != NULL)
		m_pStrData->release();
}

const char* EzString::kcharPtr(TextEncoding encoding) const
{
	if (NULL == m_pStrData)
		return "";
	if (kUnknown == encoding)
		return "";

	if (m_pStrData->m_chEncoding != encoding)
		const_cast<EzString*>(this)->copyOnWrite();

	return m_pStrData->c_str(encoding);
}

const wchar_t* EzString::kwcharPtr() const
{
	if (NULL == m_pStrData)
		return L"";

	return m_pStrData->c_wstr();
}

EzString& EzString::operator=(const EzString& other)
{
	if (this != &other)
	{
		if (m_pStrData != NULL)
			m_pStrData->release();

		m_pStrData = other.m_pStrData;
		if (m_pStrData != NULL)
			m_pStrData->addRef();
	}

	return *this;
}

EzString& EzString::operator=(const wchar_t* pwszOther)
{
	if (NULL == m_pStrData)
	{
		if (pwszOther != NULL && *pwszOther != 0)
		{
			m_pStrData = new StringData();
			EzAssert(m_pStrData != NULL);

			m_pStrData->copyString(pwszOther);
		}
	}
	else
	{
		copyOnWrite();
		m_pStrData->copyString(pwszOther);
	}

	return *this;
}

EzString& EzString::operator+=(const EzString& other)
{
	if (NULL == other.m_pStrData)
		return *this;

	if (NULL == m_pStrData)
	{
		m_pStrData = other.m_pStrData->clone();
		return *this;
	}

	copyOnWrite();
	m_pStrData->concat(other.m_pStrData);
	return *this;
}

EzString& EzString::operator+=(const wchar_t* pwszOther)
{
	if (NULL == pwszOther || 0 == *pwszOther)
		return *this;

	if (NULL == m_pStrData)
	{
		m_pStrData = new StringData();
		EzAssert(m_pStrData != NULL);

		m_pStrData->copyString(pwszOther);
		return *this;
	}

	copyOnWrite();
	m_pStrData->concat(pwszOther);
	return *this;
}

TextEncoding EzString::getEncoding() const
{
	if (NULL == m_pStrData)
		return kUnknown;

	return m_pStrData->m_chEncoding;
}

unsigned int EzString::chLength() const
{
	if (NULL == m_pStrData)
		return 0;

	const char* psz = kcharPtr(m_pStrData->m_chEncoding);
	return _mbslen((const unsigned char*)psz);
}

unsigned int EzString::strLength() const
{
	if (NULL == m_pStrData)
		return 0;

	const char* psz = kcharPtr(m_pStrData->m_chEncoding);
	return strlen(psz);
}

unsigned int EzString::wstrLength() const
{
	if (NULL == m_pStrData)
		return 0;

	const wchar_t* pwsz = kwcharPtr();
	return wcslen(pwsz);
}

int EzString::compare(const EzString& other) const
{
	if (NULL == other.m_pStrData)
	{
		if (NULL == m_pStrData)
			return 0;
		else
			return 1;
	}

	StringData* pTmp = other.m_pStrData->clone();
	EzAssert(pTmp != NULL);
	int nRet = compare(pTmp->c_wstr());
	pTmp->release();
	return nRet;
}

int EzString::compare(const wchar_t* pszOther) const
{
	const wchar_t* pwsz = kwcharPtr();
	return wcscmp(pwsz, pszOther);
}

int EzString::compareNoCase(const EzString& other) const
{
	if (NULL == other.m_pStrData)
	{
		if (NULL == m_pStrData)
			return 0;
		else
			return 1;
	}

	StringData* pTmp = other.m_pStrData->clone();
	EzAssert(pTmp != NULL);
	int nRet = compareNoCase(pTmp->c_wstr());
	pTmp->release();
	return nRet;
}

int EzString::compareNoCase(const wchar_t* pszOther) const
{
	const wchar_t* pwsz = kwcharPtr();
	return _wcsicmp(pwsz, pszOther);
}

void EzString::copyOnWrite()
{
	if (NULL == m_pStrData)
		return;

	if (m_pStrData->refCount() > 1)		// shared
	{
		m_pStrData->release();
		m_pStrData = m_pStrData->clone();
	}
}
