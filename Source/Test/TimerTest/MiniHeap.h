#ifndef __MINI_HEAP_H__
#define __MINI_HEAP_H__

#include <iostream>
using namespace std;


#define DefaultSize 10

template<class T>
class MinHeap       //最小堆的类模板实现
{
public:
	MinHeap(int sz = DefaultSize);
	MinHeap(T arr[], int n);
	~MinHeap(){ delete[]heap; }

	bool isEmpty()const{ return currentSize == 0; }
	bool isFull()const{ return currentSize == maxHeapSize; }
	void makeempty(){ currentSize = 0; }

	bool insert(const T& x); //在数组尾部插入，并调整堆
	bool removeMin(T& x);    //删除堆顶上的最小元素，最后一个元素补到堆顶，然后调整


private:
	T * heap;       //采用数组作为其存储方式。
	int currentSize;
	int maxHeapSize;

	void siftDown(int start, int m);//从start到m下滑调整为最小堆
	void siftUp(int start);         //从start到0上滑调整为最小堆
};

template<class T>
MinHeap<T>::MinHeap(int sz = DefaultSize)
{
	maxHeapSize = (sz < DefaultSize) ? DefaultSize : sz;
	heap = new T[maxHeapSize];
	if (heap == NULL){ cerr << "err\n"; exit(-1); }
	currentSize = 0;
}

template<class T>
MinHeap<T>::MinHeap(T arr[], int n)
{
	maxHeapSize = (n < DefaultSize) ? DefaultSize : n;
	heap = new T[maxHeapSize];
	if (heap == NULL){ cerr << "err\n"; exit(-1); }
	currentSize = n;

	int i = 0;
	while (i < n){                 //copy
		heap[i] = arr[i];
		++i;
	}

	int currentPos = (currentSize - 2) / 2; //找到最初调整位置，最后节点的父节点位置，也就是最后的分支节点
	while (currentPos >= 0){             //自底向上逐步扩大形成堆
		siftDown(currentPos, currentSize - 1);
		--currentPos;
	}
}

template<class T>
void MinHeap<T>::siftDown(int start, int m)//堆的下滑调整算法，从节点start到m为止，从上到下比较，如果子女小于父节点，
{                                          //关键码上浮 ，据需向下层比教，将局部子树调整为最小堆
	int i = start; int j = 2 * i + 1;    //i当前子树的根节点，j左子女
	T temp = heap[i];             //保存根节点的值
	while (j <= m){                //从上向下调整，检查是否到最后位置
		if (j < m && heap[j + 1] < heap[j])    //j指向两个子女中最小的
			++j;
		if (temp <= heap[j]) break;        //小则不做调整
		else{
			heap[i] = heap[j]; i = j; j = 2 * i + 1;//小者上移，i,j下降
		}
	}
	heap[i] = temp;
}

template<class T>
void  MinHeap<T>::siftUp(int start)//新节点插入到最小堆的后面，故需从下到上，与父节点比较，调整
{                                  //从start开始到0为止，从下向上
	int j = start, i = (j - 1) / 2;        //j表示子节点，i表示j的父节点
	int temp = heap[j];
	while (j > 0){
		if (heap[i] <= temp)break;
		else{
			heap[j] = heap[i]; j = i; i = (j - 1) / 2;
		}
		heap[j] = temp;
	}
}

template<class T>
bool MinHeap<T>::insert(const T& x)
{
	if (currentSize == maxHeapSize){
		cerr << "full\n"; return false;
	}
	heap[currentSize] = x;
	siftUp(currentSize);
	++currentSize;
	return true;
}

template<class T>
bool MinHeap<T>::removeMin(T& x)
{
	if (!currentSize){
		cerr << "empty\n"; return false;
	}
	x = heap[0];
	heap[0] = heap[currentSize - 1];
	--currentSize;
	siftDown(0, currentSize - 1);
	return true;
}

#endif // __MINI_HEAP_H__