#include "xEzDump.h"
#include <ostream>


EzDumpContext& EzDumpContext::operator<<(const char* pszInfo)
{
	m_ostream << pszInfo;
	return *this;
}

EzDumpContext& EzDumpContext::operator<<(const void* pPointer)
{
	char szBuffer[32] = { 0 };
	sprintf_s(szBuffer, EzCountOf(szBuffer), "$%p", pPointer);

	return *this << szBuffer;
}

EzDumpContext& EzDumpContext::operator<<(char c)
{
	char szBuffer[32] = { 0 };
	sprintf_s(szBuffer, EzCountOf(szBuffer), "%c", c);

	return *this << szBuffer;
}

EzDumpContext& EzDumpContext::operator<<(unsigned char uc)
{
	char szBuffer[32] = { 0 };
	sprintf_s(szBuffer, EzCountOf(szBuffer), "%d", uc);

	return *this << szBuffer;
}

EzDumpContext& EzDumpContext::operator<<(int i)
{
	char szBuffer[32] = { 0 };
	sprintf_s(szBuffer, EzCountOf(szBuffer), "%d", i);

	return *this << szBuffer;
}

EzDumpContext& EzDumpContext::operator<<(unsigned int ui)
{
	char szBuffer[32] = { 0 };
	sprintf_s(szBuffer, EzCountOf(szBuffer), "%u", ui);

	return *this << szBuffer;
}

EzDumpContext& EzDumpContext::operator<<(short s)
{
	char szBuffer[32] = { 0 };
	sprintf_s(szBuffer, EzCountOf(szBuffer), "%d", s);

	return *this << szBuffer;
}

EzDumpContext& EzDumpContext::operator<<(unsigned short us)
{
	char szBuffer[32] = { 0 };
	sprintf_s(szBuffer, EzCountOf(szBuffer), "%u", us);

	return *this << szBuffer;
}

EzDumpContext& EzDumpContext::operator<<(long l)
{
	char szBuffer[32] = { 0 };
	sprintf_s(szBuffer, EzCountOf(szBuffer), "%d", l);

	return *this << szBuffer;
}

EzDumpContext& EzDumpContext::operator<<(unsigned long ul)
{
	char szBuffer[32] = { 0 };
	sprintf_s(szBuffer, EzCountOf(szBuffer), "%u", ul);

	return *this << szBuffer;
}

EzDumpContext& EzDumpContext::operator<<(EzInt64 i64)
{
	char szBuffer[32] = { 0 };
	sprintf_s(szBuffer, EzCountOf(szBuffer), "%I64d", i64);

	return *this << szBuffer;
}

EzDumpContext& EzDumpContext::operator<<(EzUInt64 ui64)
{
	char szBuffer[32] = { 0 };
	sprintf_s(szBuffer, EzCountOf(szBuffer), "%I64u", ui64);

	return *this << szBuffer;
}

EzDumpContext& EzDumpContext::operator<<(float f)
{
	char szBuffer[32] = { 0 };
	sprintf_s(szBuffer, EzCountOf(szBuffer), "%f", f);

	return *this << szBuffer;
}

EzDumpContext& EzDumpContext::operator<<(double d)
{
	char szBuffer[32] = { 0 };
	sprintf_s(szBuffer, EzCountOf(szBuffer), "%f", d);

	return *this << szBuffer;
}

void EzDumpContext::flush()
{
	m_ostream.flush();
}
