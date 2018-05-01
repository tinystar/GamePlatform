#include <iostream>
#include "xEzUtil.h"
#include <conio.h>
#include <windows.h>
#include "MinHeap.h"

#pragma comment(lib, "winmm.lib")

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

void TimerServiceTestEntry();

int main()
{
// 	srand(::GetTickCount());
// 
// 	cout << "press any key~~" << endl;
// 	_getch();
// 
// 	testNewPerf();
// 
// 	for (int i = 0; i < 10; ++i)
// 		testMemPerf();
// 
// 	time_t tArr[1000];
// 	for (int i = 0; i < 1000; ++i)
// 	{
// 		tArr[i] = time(NULL);
// 	}

// 	MinHeap<int> * heap = new MinHeap<int >(8);
// 	heap->Insert(53);
// 	heap->Insert(17);
// 	heap->Insert(78);
// 	int value;
// 	heap->RemoveMin(value);
// 	cout << value << " \n";
// 
// 	heap->RemoveMin(value);
// 	cout << value << " \n";
// 
// 	heap->RemoveMin(value);
// 	cout << value << " \n";
// 	heap->Insert(53);
// 	heap->Insert(17);
// 	heap->Insert(78);
// 	heap->Insert(9);
// 	heap->Insert(45);
// 	heap->Insert(65);
// 	heap->Insert(87);
// 	heap->Insert(23);
// 	cout << "依次删除最小堆中的元素<从小到大>：\n" << endl;
// 	int value;
// 	heap->RemoveMin(value);
// 	cout << value << " ";
// 
// 	heap->RemoveMin(value);
// 	cout << value << " ";
// 
// 	heap->RemoveMin(value);
// 	cout << value << " ";
// 
// 	heap->RemoveMin(value);
// 	cout << value << " ";
// 
// 	heap->RemoveMin(value);
// 	cout << value << " ";
// 
// 
// 	heap->RemoveMin(value);
// 	cout << value << " ";
// 
// 	heap->RemoveMin(value);
// 	cout << value << " ";
// 
// 	heap->RemoveMin(value);
// 	cout << value << " \n\n";
// 	delete heap;
	
// 	TIMECAPS	timeCaps;
// 	timeGetDevCaps(&timeCaps, sizeof(timeCaps));
// 	UINT iPeriodMin = timeCaps.wPeriodMin;
// 
// 	//MMRESULT ret = timeBeginPeriod(1);
// 	EzStopwatch watch1;
// 	for (int i = 0; i < 10; ++i)
// 	{
// 		watch1.start();
// 		::Sleep(5);
// 		cout << "period: " << watch1.stop() << endl;
// 	}
// 	//timeEndPeriod(1);

	TimerServiceTestEntry();

	_getch();
	return 0;
}