#ifndef __MINI_HEAP_H__
#define __MINI_HEAP_H__

#include <iostream>
using namespace std;


#define DefaultSize 10

template<class T>
class MinHeap       //��С�ѵ���ģ��ʵ��
{
public:
	MinHeap(int sz = DefaultSize);
	MinHeap(T arr[], int n);
	~MinHeap(){ delete[]heap; }

	bool isEmpty()const{ return currentSize == 0; }
	bool isFull()const{ return currentSize == maxHeapSize; }
	void makeempty(){ currentSize = 0; }

	bool insert(const T& x); //������β�����룬��������
	bool removeMin(T& x);    //ɾ���Ѷ��ϵ���СԪ�أ����һ��Ԫ�ز����Ѷ���Ȼ�����


private:
	T * heap;       //����������Ϊ��洢��ʽ��
	int currentSize;
	int maxHeapSize;

	void siftDown(int start, int m);//��start��m�»�����Ϊ��С��
	void siftUp(int start);         //��start��0�ϻ�����Ϊ��С��
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

	int currentPos = (currentSize - 2) / 2; //�ҵ��������λ�ã����ڵ�ĸ��ڵ�λ�ã�Ҳ�������ķ�֧�ڵ�
	while (currentPos >= 0){             //�Ե������������γɶ�
		siftDown(currentPos, currentSize - 1);
		--currentPos;
	}
}

template<class T>
void MinHeap<T>::siftDown(int start, int m)//�ѵ��»������㷨���ӽڵ�start��mΪֹ�����ϵ��±Ƚϣ������ŮС�ڸ��ڵ㣬
{                                          //�ؼ����ϸ� ���������²�Ƚ̣����ֲ���������Ϊ��С��
	int i = start; int j = 2 * i + 1;    //i��ǰ�����ĸ��ڵ㣬j����Ů
	T temp = heap[i];             //������ڵ��ֵ
	while (j <= m){                //�������µ���������Ƿ����λ��
		if (j < m && heap[j + 1] < heap[j])    //jָ��������Ů����С��
			++j;
		if (temp <= heap[j]) break;        //С��������
		else{
			heap[i] = heap[j]; i = j; j = 2 * i + 1;//С�����ƣ�i,j�½�
		}
	}
	heap[i] = temp;
}

template<class T>
void  MinHeap<T>::siftUp(int start)//�½ڵ���뵽��С�ѵĺ��棬������µ��ϣ��븸�ڵ�Ƚϣ�����
{                                  //��start��ʼ��0Ϊֹ����������
	int j = start, i = (j - 1) / 2;        //j��ʾ�ӽڵ㣬i��ʾj�ĸ��ڵ�
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