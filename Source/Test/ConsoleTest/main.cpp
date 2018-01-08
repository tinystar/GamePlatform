#include <iostream>
#include "xEzUtil.h"
#include <conio.h>
#include <windows.h>

using namespace std;

int rangeRand(int low, int high)
{
	return low + rand() % (high - low);
}

void testMemPerf()
{
	EzStopwatch watch1, watch2;

	watch1.start();
	for (int i = 0; i < 10000; ++i)
	{
		int nAllocSize = rangeRand(1, 100);
		void* pTest = EzMalloc(nAllocSize);
		EzFree(pTest);
	}
	cout << "------memory pool alloc time: " << watch1.stop() << "--------" << endl;

	watch2.start();
	for (int i = 0; i < 10000; ++i)
	{
		int nAllocSize = rangeRand(1, 100);
		void* pTest = malloc(nAllocSize);
		free(pTest);
	}
	cout << "------system memory alloc time: " << watch2.stop() << "--------" << endl;
}

class A : public EzHeapOper
{
private:
	int mI[10];
};

class B
{
private:
	int mI[10];
};

void testNewPerf()
{
	EzStopwatch watch1, watch2;

	watch1.start();
	for (int i = 0; i < 10000; ++i)
	{
		A* pA = new A();
		delete pA;
	}
	cout << "------EzHeapOper time: " << watch1.stop() << "--------" << endl;

	watch2.start();
	for (int i = 0; i < 10000; ++i)
	{
		B* pB = new B();
		delete pB;
	}
	cout << "------system new time: " << watch2.stop() << "--------" << endl;
}

int main()
{
	srand(::GetTickCount());

	cout << "press any key~~" << endl;
	_getch();

	testNewPerf();

	for (int i = 0; i < 10; ++i)
		testMemPerf();

	time_t tArr[1000];
	for (int i = 0; i < 1000; ++i)
	{
		tArr[i] = time(NULL);
	}

	_getch();
	return 0;
}