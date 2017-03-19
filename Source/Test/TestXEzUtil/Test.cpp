#include "stdafx.h"
#include "Test.h"
#include "xEzUtil.h"
#include <process.h>
#include <fstream>
#include "TestDef.h"
#include <xmemory>
#include <sstream>
#include <map>

using namespace std;
using namespace EzText;
using namespace EzSys;

class MyLog
{
public:
	MyLog(const TCHAR* pszLogFile);
	~MyLog();

	void log(const TCHAR* pszFormat, ...);
	void flush() { m_LogFile.flush(); }

private:
	std::ofstream		m_LogFile;
	EzLock				m_lock;
};

MyLog::MyLog(const TCHAR* pszLogFile)
{
	m_LogFile.open(pszLogFile, ios::app);
	EzAssert(m_LogFile);
}

MyLog::~MyLog()
{
	if (m_LogFile)
		m_LogFile.close();
}

void MyLog::log(const TCHAR* pszFormat, ...)
{
	EzAutoLocker autoLocker(&m_lock);

	va_list vargs;
	va_start(vargs, pszFormat);

	int len = _vsctprintf(pszFormat, vargs);
	TCHAR* pszLogBuf = (TCHAR*)_alloca((len + 1)*sizeof(TCHAR));
	len = _vsntprintf_s(pszLogBuf, len + 1, len, pszFormat, vargs);
	*(pszLogBuf + len) = 0;

	auto_ptr<char> strUtf8(tcharToUtf8(pszLogBuf));
	
	m_LogFile.write("[INFO]", 6);
	m_LogFile.write(strUtf8.get(), strlen(strUtf8.get()));
	//m_LogFile.flush();
}

MyLog mylog(_T("MyLog.log"));


unsigned __stdcall testWriteLogThread(void* pParam)
{
	HANDLE hCurThread = ::GetCurrentThread();

	for (int i = 0; i < 500; ++i)
	{
		EzLog(kLogDebug, _T("thread[%d] test log %d, %s is writtern by %s\n"),
									::GetThreadId(hCurThread),
									i,
									_T("xEzUtil"),
									_T("WangHaixin"));
		::Sleep(1);
	}

	EzLog(kLogInfo, _T("�߳�[%d]׼���˳�\n"), ::GetThreadId(hCurThread));
	EzLog(kLogDebug, _T("����~byebye����\n"));
	return 0;
}

void testLog()
{
	EzLog(kLogInfo, _T("begin create test log thread\n"));

	HANDLE hThreads[3] = { 0 };
	for (int i = 0; i < 3; ++i)
	{
		HANDLE hTestLogThread = (HANDLE)_beginthreadex(NULL, 0, testWriteLogThread, NULL, 0, NULL);
		hThreads[i] = hTestLogThread;
	}
	::WaitForMultipleObjects(3, hThreads, TRUE, INFINITE);
	for (int i = 0; i < 3; ++i)
		::CloseHandle(hThreads[i]);

	EzLog(kLogInfo, _T("end create test log thread\n"));

	for (int i = 0; i < 20; ++i)
	{
		EzLogDebug(_T("main thread log debug test %d\n"), i);
	}
}

// unsigned __stdcall testWriteLogThread1(void* pParam)
// {
// 	HANDLE hCurThread = ::GetCurrentThread();
// 
// 	for (int i = 0; i < 500; ++i)
// 	{
// 		mylog.log(_T("thread[%d] test log %d, %s is writtern by %s\n"),
// 			::GetThreadId(hCurThread),
// 			i,
// 			_T("xEzUtil"),
// 			_T("WangHaixin"));
// 		::Sleep(1);
// 	}
// 
// 	mylog.log(_T("�߳�[%d]׼���˳�\n"), ::GetThreadId(hCurThread));
// 	mylog.log(_T("����~byebye����\n"));
// 	return 0;
// }
// 
// void testLog1()
// {
// 	mylog.log(_T("begin create test log thread\n"));
// 
// 	HANDLE hThreads[3] = { 0 };
// 	for (int i = 0; i < 3; ++i)
// 	{
// 		HANDLE hTestLogThread = (HANDLE)_beginthreadex(NULL, 0, testWriteLogThread1, NULL, 0, NULL);
// 		hThreads[i] = hTestLogThread;
// 	}
// 	::WaitForMultipleObjects(3, hThreads, TRUE, INFINITE);
// 	for (int i = 0; i < 3; ++i)
// 		::CloseHandle(hThreads[i]);
// 
// 	mylog.log(_T("end create test log thread\n"));
// 
// 	for (int i = 0; i < 20; ++i)
// 	{
// 		mylog.log(_T("main thread log debug test %d\n"), i);
// 	}
// }

void testLogPerformance()
{
	EzStopwatch watch;
	//EzInt64 elapse;

	watch.start();
	for (int i = 0; i < 1500; ++i)
	{
		mylog.log(_T("[%d] test log abc hello -- %s\n"), i, _T("test"));
		//::Sleep(0);
		mylog.log(_T("     world ttttt %s\n"), _T("test12311111"));
	}
	//mylog.flush();
	EzInt64 elapse = watch.stop();
	EzTrace(_T("my log time = %I64d"), elapse);

	//watch.start();
	//::Sleep(1000);
	//elapse = watch.stop();
	//EzTrace(_T("test1 = %I64d"), elapse);

	EzLogFlush();

	watch.start();
	for (int i = 0; i < 1500; ++i)
	{
		EzLog(kLogInfo, _T("[%d] test log abc hello -- %s\n"), i, _T("test"));
		//::Sleep(0);
		EzLog(kLogInfo, _T("     world ttttt %s\n"), _T("test12311111"));
	}
	elapse = watch.stop();
	EzTrace(_T("ezlog time = %I64d"), elapse);

// 	watch.start();
// 	testLog();
// 	elapse = watch.stop();
// 	EzTrace(_T("test log = %I64d"), elapse);
// 
// 	watch.start();
// 	testLog1();
// 	elapse = watch.stop();
// 	EzTrace(_T("test log1 = %I64d"), elapse);
}

void testDebug()
{
	int t = 0;
	EzAssert(t == 0);
// 	EzAssert(t != 0);
// 	if (EzVerify(0 == t))
// 		::MessageBox(NULL, _T("0 == t"), _T("test"), MB_OK);

	EzTrace(_T("test trace: %s %d %f"), _T("trace"), 10, .1);
}

void testBuffer()
{
	EzFixedBuf<char, 5> buf;
	int i = 5;
	buf.write((char*)&i, sizeof(int));

	EzFixedBuf<char, 5> other(buf);
	EzFixedBuf<char, 5> other1;
	other1 = buf;

	ofstream ofile("testbuf.txt", ios::trunc);
	EzFixedBuf<int, 10> intBuf;
	int ia[5] = { 1, 2, 3, 4, 5 };
	intBuf.write(ia, 5);
	intBuf.flush(ofile);
// 	EzFixedBuf<10> other2(buf);
}

void testException()
{
	try
	{
		throw EzException(_T("test exception"), _T(__FILE__), __LINE__);
	}
	catch (EzException e)
	{
		EzTrace(e.info());
		EzTrace(e.file());
	}
	catch (...)
	{

	}
}

void testTxtCvtor()
{
	const wchar_t* pTest1 = L"abc\nefd�й���\n";
	const char* pTest2 = "abc\nefd�й���\n";

	auto_ptr<char> utf8Ptr(wideCharToUtf8(pTest1));
	auto_ptr<char> utf8Ptr2(ansiToUtf8(pTest2));
	auto_ptr<wchar_t> unicodePtr(ansiToWideChar(pTest2));
	auto_ptr<char> ansiPtr(wideCharToAnsi(pTest1));
	auto_ptr<char> utf8Ptr3(tcharToUtf8(_T("test convertor")));
}

void testSystem()
{
	bool bExist = pathFileExists(_T(".\\a\\1"));
	bExist = pathFileExists(_T("./b"));
	bExist = pathFileExists(_T("a"));
	bExist = pathFileExists(_T("a/2"));
	bExist = pathFileExists(_T("f:\\abx"));

// 	bool bSuccess = createDirectory(_T(".\\a\\1"));
// 	bSuccess = createDirectory(_T("b"));
// 	bSuccess = createDirectory(_T("./c"));
// 	bSuccess = createDirectory(_T("\\c"));
// 	bSuccess = createDirectory(_T("\\d\\"));
// 	bSuccess = createDirectory(_T("f:\\abx"));
// 	bSuccess = createDirectory(_T("f:\\abx/test\\"));
}

void testLinkedList()
{
	typedef EzDoublyLinkedList<int> IntLinkedList;
	
	IntLinkedList testList1;

	IntLinkedList::Node* pNode = new IntLinkedList::Node;
	pNode->value = 1;
	testList1.push_back(pNode);

	pNode = new IntLinkedList::Node;
	IntLinkedList::Node* pNode2 = pNode;
	pNode->value = 2;
	testList1.push_back(pNode);

	pNode = new IntLinkedList::Node;
	pNode->value = 0;
	testList1.push_front(pNode);

	pNode = new IntLinkedList::Node;
	pNode->value = 3;
	testList1.push_back(pNode);

	pNode = new IntLinkedList::Node;
	pNode->value = 5;
	IntLinkedList::Iterator iter = testList1.end();
	testList1.insert(iter, pNode);
	
	iter = testList1.tail();

	// ��������ʧЧ
	IntLinkedList::Node* pNode1 = testList1.pop_back();
	delete pNode1;

	pNode = new IntLinkedList::Node;
	pNode->value = 22;
	iter = testList1.head();
	++iter;

	testList1.insert(iter++, pNode);

	pNode = new IntLinkedList::Node;
	pNode->value = 222;
	testList1.insert(iter, pNode);

	size_t elemCount = testList1.size();
	bool bempty = testList1.isEmpty();

	testList1.remove(pNode2);
	delete pNode2;
	testList1.remove(pNode);
	delete pNode;
	IntLinkedList::Node* pFirst = testList1.head().operator->();
	testList1.remove(pFirst);
	delete pFirst;

	IntLinkedList::Iterator itrNext = testList1.erase(testList1.tail());

	elemCount = testList1.size();

	testList1.clear();

	bempty = testList1.isEmpty();

	typedef EzDoublyLinkedList<TestNC> EzTestNCList;

	EzTestNCList TestNCList;

	EzTestNCList::Node* pTNCNode = new EzTestNCList::Node;
	pTNCNode->value.setNum(0);
	pTNCNode->value.setString(_T("test 0"));
	TestNCList.push_back(pTNCNode);

	pTNCNode = new EzTestNCList::Node;
	pTNCNode->value.setNum(1);
	pTNCNode->value.setString(_T("test 1"));
	TestNCList.push_back(pTNCNode);

	pTNCNode = new EzTestNCList::Node;
	pTNCNode->value.setNum(-1);
	pTNCNode->value.setString(_T("test head"));
	TestNCList.push_front(pTNCNode);

	EzTestNCList::Iterator tncitr = TestNCList.head();
	while (tncitr != TestNCList.end())
	{
		tncitr->value.print();
		++tncitr;
	}
}

class DTest
{
public:
	DTest(int a, float b)
		: m_nA(a)
		, m_fB(b)
	{}

	void dump(EzDumpContext& dc);

private:
	int m_nA;
	float m_fB;
};

void DTest::dump(EzDumpContext& dc)
{
	dc << "DTest: this = " << this << "\tA = " << m_nA << "\tB = " << m_fB << "\n";
	dc << "------------DTest dump over!------------";
}

void testDump()
{
	DTest tobj(5, .5);

	stringstream ss;
	EzDumpContext dc(ss);
	tobj.dump(dc);

	string info = ss.str();

	ofstream ofile("dump.txt");
	ofile << info;
}

void testEzArray()
{
	EzArray<int>	iArr;
	iArr.append(1);
	iArr.append(2);
	iArr.append(3);

	iArr.setLogicalLength(2);
}

void testMapPerf()
{
	srand(::GetTickCount());

	std::map<int, int> map1, map2, map3;

	for (int i = 0; i < 40000; ++i)
	{
		map1.insert(std::map<int, int>::value_type(i, 0));
	}

	for (int i = 0; i < 40000; ++i)
	{
		int num = rand() % RAND_MAX;
		if (i >= RAND_MAX)
			num += RAND_MAX;
		do 
		{
			if (map2.find(num) == map2.end())
				break;
			num = rand() % INT_MAX;
			if (i >= RAND_MAX)
				num += RAND_MAX;
		} while (true);

		map2.insert(std::map<int, int>::value_type(num, 0));
	}

	for (std::map<int, int>::iterator iter = map2.begin(); iter != map2.end(); ++iter)
		map3.insert(std::map<int, int>::value_type(iter->first, iter->second));

	EzStopwatch watch;
	EzTrace(_T("---------map1----------"));
	watch.start();
	map1.clear();
	EzTrace(_T("---------map1: %d------"), watch.stop());

	EzTrace(_T("---------map2----------"));
	watch.start();
	map2.clear();
	EzTrace(_T("---------map2: %d------"), watch.stop());

	EzTrace(_T("---------map3----------"));
	watch.start();
	map3.clear();
	EzTrace(_T("---------map3: %d------"), watch.stop());
}

EzRWLock g_rwLock;

unsigned __stdcall testRWLockThread(void* pParam)
{
	TCHAR sz[50] = { 0 };
	_stprintf_s(sz, _T("RWLock Test: Thread %d"), PtrToShort(pParam));
	int n = ::MessageBox(NULL, _T("Yes: Attempt to read\nNo: Attempt to write"), sz, MB_YESNO);
	if (n == IDYES)
		g_rwLock.lockRead();
	else
		g_rwLock.lockWrite();

	::MessageBox(NULL, (n == IDYES) ? _T("OK stops Reading") : _T("OK stops Writing"), sz, MB_OK);

	if (n == IDYES)
		g_rwLock.unlockRead();
	else
		g_rwLock.unlockWrite();

	return 0;
}

void testRWLock()
{
	HANDLE hThreads[5] = { 0 };
	for (int i = 0; i < 5; ++i)
	{
		hThreads[i] = (HANDLE)_beginthreadex(NULL, 0, testRWLockThread, (void*)i, 0, NULL);
	}
	::WaitForMultipleObjects(8, hThreads, TRUE, INFINITE);
	for (int i = 0; i < 5; ++i)
		::CloseHandle(hThreads[i]);
}

void testEntry()
{
	// debug
	testDebug();
	// buffer
	testBuffer();
	// exception
	testException();
	// text convertor
	testTxtCvtor();
	// log
	//testLog();
	testLogPerformance();
	// system
	testSystem();
	// linked list
	testLinkedList();
	// dump test
	testDump();
	// EzArray test
	testEzArray();
	// std map performance test
	testMapPerf();
	// test RWLock
	testRWLock();
}
