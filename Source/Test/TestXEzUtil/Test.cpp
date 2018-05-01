#include "stdafx.h"
#include "Test.h"
#include "xEzUtil.h"
#include <process.h>
#include <fstream>
#include "TestDef.h"
#include <xmemory>
#include <sstream>
#include <map>
#include "xEzStringImp.h"

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

	EzLog(kLogInfo, _T("线程[%d]准备退出\n"), ::GetThreadId(hCurThread));
	EzLog(kLogDebug, _T("哈哈~byebye！！\n"));
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
// 	mylog.log(_T("线程[%d]准备退出\n"), ::GetThreadId(hCurThread));
// 	mylog.log(_T("哈哈~byebye！！\n"));
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
	double elapse = watch.stop();
	EzTrace(_T("my log time = %f"), elapse);

	//watch.start();
	//::Sleep(1000);
	//elapse = watch.stop();
	//EzTrace(_T("test1 = %f"), elapse);

	EzLogFlush();

	watch.start();
	for (int i = 0; i < 1500; ++i)
	{
		EzLog(kLogInfo, _T("[%d] test log abc hello -- %s\n"), i, _T("test"));
		//::Sleep(0);
		EzLog(kLogInfo, _T("     world ttttt %s\n"), _T("test12311111"));
	}
	elapse = watch.stop();
	EzTrace(_T("ezlog time = %f"), elapse);

// 	watch.start();
// 	testLog();
// 	elapse = watch.stop();
// 	EzTrace(_T("test log = %f"), elapse);
// 
// 	watch.start();
// 	testLog1();
// 	elapse = watch.stop();
// 	EzTrace(_T("test log1 = %f"), elapse);
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
	const wchar_t* pTest1 = L"abc\nefd中国人\n";
	const char* pTest2 = "abc\nefd中国人\n";

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

	// 迭代器会失效
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
	EzTrace(_T("---------map1: %f------"), watch.stop());

	EzTrace(_T("---------map2----------"));
	watch.start();
	map2.clear();
	EzTrace(_T("---------map2: %f------"), watch.stop());

	EzTrace(_T("---------map3----------"));
	watch.start();
	map3.clear();
	EzTrace(_T("---------map3: %f------"), watch.stop());
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

class TestNew : public EzHeapOper
{
private:
	int m_i1;
	short m_s1;
};

void testMemory()
{
	void* p1 = EzMalloc(1);
	void* p2 = EzMalloc(3);
	void* p3 = EzMalloc(4);
	void* p4 = EzMalloc(190);
	void* p5 = EzMalloc(192);
	void* p6 = EzMalloc(250);
	void* p7 = EzMalloc(256);
	void* p8 = EzMalloc(400);
	void* p9 = EzMalloc(500);
	void* p10 = EzMalloc(512);

	EzFree(p1);
	EzFree(p2);
	EzFree(p3);
	EzFree(p4);
	EzFree(p5);
	EzFree(p6);
	EzFree(p7);
	EzFree(p8);
	EzFree(p9);
	EzFree(p10);

 	TestNew* pT1 = new TestNew();
 	delete pT1;
}

int rangeRand(int low, int high)
{
	return low + rand() % (high - low);
}

unsigned __stdcall testMemPerfThread1(void* pParam)
{
	EzStopwatch watch;
	watch.start();

	for (int i = 0; i < 10000; ++i)
	{
		int nSize = rangeRand(1, 200);
		void* pTest = EzMalloc(nSize);
		EzFree(pTest);
	}

	MyTrace(_T("-------memory pool time: %f-------\n"), watch.stop());
	return 0;
}

unsigned __stdcall testMemPerfThread2(void* pParam)
{
	EzStopwatch watch;
	watch.start();

	for (int i = 0; i < 10000; ++i)
	{
		int nSize = rangeRand(1, 200);
		void* pTest = malloc(nSize);
		free(pTest);
	}

	MyTrace(_T("-------system malloc time: %f-------\n"), watch.stop());
	return 0;
}

void testMemPerf()
{
	EzStopwatch watch1, watch2;

	watch1.start();

	HANDLE hThreads[5] = { 0 };
	for (int i = 0; i < 5; ++i)
	{
		HANDLE hTestMemThread = (HANDLE)_beginthreadex(NULL, 0, testMemPerfThread1, NULL, 0, NULL);
		hThreads[i] = hTestMemThread;
	}
	::WaitForMultipleObjects(5, hThreads, TRUE, INFINITE);

	EzTrace(_T("-------memory pool alloc total time: %f-------"), watch1.stop());

	watch2.start();

	for (int i = 0; i < 5; ++i)
	{
		HANDLE hTestMemThread = (HANDLE)_beginthreadex(NULL, 0, testMemPerfThread2, NULL, 0, NULL);
		hThreads[i] = hTestMemThread;
	}
	::WaitForMultipleObjects(5, hThreads, TRUE, INFINITE);

	EzTrace(_T("-------system malloc alloc total time: %f-------"), watch2.stop());
}

void testEzString()
{
	StringBuffer<char> cEmpty;
	StringBuffer<char> cEmpty1(NULL, 0);
	StringBuffer<char> testS("123", 3);
	StringBuffer<wchar_t> testWS(L"123", 3);
	StringBuffer<char> testS1(testS);
	StringBuffer<wchar_t> testWS1(L"abcdef");

	const char* pszEmpt = cEmpty;
	cEmpty += "hello";
	cEmpty.copyStrData('A', 0);
	cEmpty.reset();
	cEmpty = testS + testS1;
	cEmpty = "123" + cEmpty;
	pszEmpt = cEmpty;

	if (testS == testS1)
	{
		if ("123" == testS)
			testS += "456";
		testS += testS;
	}

	testWS = testWS1;
	testWS = L"AAAA";
	testWS += L"BBB";
	if (testWS == L"AAAABBB")
		testWS = testWS1 + L"111";
	else
		testWS = testWS1 + L"222";

	if (testWS1.compareNoCase(L"AbcDEF"))
		testWS1.reset();

	const wchar_t* pTestWS1 = testWS;

	EzString strEmpty;
	EzString strW1 = L"仗剑走天涯，何处是我家？";
	EzString strA1("仗剑走天涯，何处是我家？", kAnsi);
	EzString strW2(L'A', 5);
	EzString strA2('A', 5);

	const wchar_t* pWRet = NULL;
	const char* pRet = NULL;
	strEmpty = strW1;
	strEmpty += L"abc";
	strEmpty = L"new string";
	EzString strEmpt1;
	strEmpt1 = L"ttt";
	EzString strEmpt2;
	strEmpt2 += L"aaa";
	EzString strW3 = strW1;
	pWRet = strW1.kwcharPtr();
	pRet = strW1.kcharPtr(kAnsi);
	pRet = strW3.kcharPtr(kUtf8);

	strW1 += pWRet;
	strW1 += strA1;
	strW1 += strW3;
	EzString strT = strW1 + strW3 + strA1;
	EzString strT1 = strW1 + strEmpty;

	strT = L"abc";
	if (strT == strW1)
	{
		strT = L"";
	}
	else if (strT == L"123")
	{
		strT = L"";
	}
	else if (L"abc" == strT)
	{
		strT.kcharPtr(kUtf8);
		strT = L"";
	}

	TextEncoding enc = strW1.getEncoding();
	enc = strW3.getEncoding();
	unsigned int uLen = strW3.chLength();
	uLen = strW3.strLength();
	uLen = strW3.wstrLength();
	strW3.kcharPtr(kAnsi);
	uLen = strA1.chLength();
	uLen = strA1.strLength();
}

void testSmartPtr()
{
	EzSharedPtr<int> ptrTest1(new int(5));
	EzSharedPtr<int> ptrTest2;
	ptrTest2 = ptrTest1;
	EzSharedPtr<int> ptrTest3(ptrTest2);

	ptrTest2 = new int(0);

	int nRet = *ptrTest1;
	nRet = *ptrTest2;

	if (ptrTest2)
	{
		EzSharedPtr<std::string> ptrStr(new std::string("abc"));
		char ch = ptrStr->operator[](1);
		const char* psz = ptrStr->c_str();
		ptrStr.reset();
		ptrStr.refCount();
	}

	ptrTest1.refCount();
}

void testMiniHeap()
{
	EzMiniHeap<int> testheap(5);
	bool bIsEmpty = testheap.isEmpty();
	testheap.insert(53);
	testheap.insert(17);
	testheap.insert(78);
	testheap.insert(9);
	testheap.insert(45);
	testheap.insert(65);
	testheap.insert(87);
	testheap.insert(23);

	bIsEmpty = testheap.isEmpty();

	int arr[] = { 53, 17, 78, 9, 45, 65, 87, 23 };
	EzMiniHeap<int> testheap1(arr, EzCountOf(arr));

	const EzArray<int>& heapArr = testheap1.toArray();
	for (int i = 0; i < heapArr.logicalLength(); ++i)
	{
		int item = heapArr[i];
	}

	int value;
	testheap.removeMin(value);
	testheap.removeMin(value);
	testheap.removeMin(value);
	testheap.removeMin(value);
	testheap.removeMin(value);
	testheap.removeMin(value);
	testheap.removeMin(value);
	testheap.removeMin(value);
	//testheap.removeMin(value);
	testheap.insert(17);
	testheap.insert(8);
	testheap.insert(45);

	int nSize = testheap.size();

	testheap.clear();
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
// 	testRWLock();
	// test Memory Allocate
	//testMemory();
 	//testMemPerf();

	testEzString();
	testSmartPtr();

	testMiniHeap();
}
