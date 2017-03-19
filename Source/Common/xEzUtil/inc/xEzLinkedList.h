/************************************************************************/
/* All rights reserved.
/* You can't use, copy or modify the file without the permission of the
/* author. Otherwise I will be held accountable under the law.
/*
/* Author: xiaoxin
/* Contact: 765392700(QQ)
/* Date: 2017/1/8
/************************************************************************/
#ifndef __XEZ_LINKED_LIST_H__
#define __XEZ_LINKED_LIST_H__

#include "xEzMacro.h"
#include "xEzException.h"

// carefully to be used.
template<class T>
class EzDoublyLinkedList
{
public:
	struct Node
	{
		T		value;
		Node*	prev;
		Node*	next;

		Node()
			: prev(NULL)
			, next(NULL)
		{}

		Node(const T& val)
			: value(val)
			, prev(NULL)
			, next(NULL)
		{}
	};

	class Iterator
	{
	public:
		Iterator()
			: m_pNode(NULL)
		{}

		Iterator(Node* pNode)
			: m_pNode(pNode)
		{}

		Node* current()
		{
			return m_pNode;
		}

		Node* operator-> ()
		{
			return m_pNode;
		}

		Node& operator* ()
		{
			return *m_pNode;
		}

		Iterator& operator++ ()
		{
			if (NULL == m_pNode)
				throw EzException(_T("Null pointer!"), _T(__FILE__), __LINE__);

			m_pNode = m_pNode->next;
			return *this;
		}

		Iterator operator++ (int)
		{
			Iterator ret(*this);
			++(*this);
			return ret;
		}

		Iterator operator-- ()
		{
			if (NULL == m_pNode)
				throw EzException(_T("Null pointer!"), _T(__FILE__), __LINE__);

			m_pNode = m_pNode->prev;
			return *this;
		}

		Iterator operator-- (int)
		{
			Iterator ret(*this);
			--(*this);
			return ret;
		}

		bool operator== (const Iterator& other)
		{
			return m_pNode == other.m_pNode;
		}

		bool operator!= (const Iterator& other)
		{
			return m_pNode != other.m_pNode;
		}

	private:
		Node*	m_pNode;
	};

	typedef EzDoublyLinkedList<T>	MyType;

public:
	EzDoublyLinkedList()
		: m_pHead(NULL)
		, m_pTail(NULL)
		, m_nSize(0)
	{}

	~EzDoublyLinkedList()
	{
		clear();
	}

private:
	EzDoublyLinkedList(const MyType& right);
	MyType& operator= (const MyType& right);

public:
	size_t	size() const { return m_nSize; }
	bool	isEmpty() const { return m_nSize == 0; }

	void	clear();

	Iterator head() const { return Iterator(m_pHead); }
	Iterator tail() const { return Iterator(m_pTail); }
	Iterator end() const { return Iterator(NULL); }

	void push_back(Node* pNode);
	void push_front(Node* pNode);
	Node* pop_back();
	Node* pop_front();

	// insert before iterator, if itr is end, then insert to the end of the list.
	void insert(Iterator itr, Node* pNode);

	// only remove pNode from the list, not delete the pNode.
	void remove(Node* pNode);

	// remove and delete the node that itr is referenced. returns Iterator that reference to the next node.
	Iterator erase(Iterator itr);

private:
	Node*	m_pHead;
	Node*	m_pTail;
	size_t	m_nSize;
};


template<class T>
void EzDoublyLinkedList<T>::clear()
{
	Node* pNode = m_pHead;
	while (pNode)
	{
		Node* pTmp = pNode;
		pNode = pNode->next;
		delete pTmp;
	}

	m_pHead = m_pTail = NULL;
	m_nSize = 0;
}

template<class T>
void EzDoublyLinkedList<T>::push_back(Node* pNode)
{
	if (NULL == pNode)
		return;

	if (NULL == m_pTail)
	{
		EzAssert(m_pHead == NULL);
		EzAssert(m_nSize == 0);
		m_pHead = m_pTail = pNode;
		m_pHead->next = m_pTail->next = NULL;
		m_pHead->prev = m_pTail->prev = NULL;
	}
	else
	{
		EzAssert(m_pHead != NULL);
		EzAssert(m_nSize > 0);
		m_pTail->next = pNode;
		pNode->prev = m_pTail;
		pNode->next = NULL;
		m_pTail = pNode;
	}
	++m_nSize;
}

template<class T>
void EzDoublyLinkedList<T>::push_front(Node* pNode)
{
	if (NULL == pNode)
		return;

	if (NULL == m_pHead)
	{
		EzAssert(m_pTail == NULL);
		EzAssert(m_nSize == 0);
		m_pHead = m_pTail = pNode;
		m_pHead->next = m_pTail->next = NULL;
		m_pHead->prev = m_pTail->prev = NULL;
	}
	else
	{
		EzAssert(m_pTail != NULL);
		EzAssert(m_nSize > 0);
		m_pHead->prev = pNode;
		pNode->next = m_pHead;
		pNode->prev = NULL;
		m_pHead = pNode;
	}
	++m_nSize;
}

template<class T>
typename EzDoublyLinkedList<T>::Node* EzDoublyLinkedList<T>::pop_back()
{
	if (NULL == m_pTail)
	{
		EzAssert(m_pHead == NULL);
		EzAssert(m_nSize == 0);
		return NULL;
	}
	else
	{
		Node* pNode = m_pTail;
		m_pTail = m_pTail->prev;
		if (NULL == m_pTail)
		{
			m_pHead = m_pTail = NULL;
			EzAssert(m_nSize == 1);
			m_nSize = 0;
		}
		else
		{
			m_pTail->next = NULL;
			--m_nSize;
		}

		pNode->next = pNode->prev = NULL;
		return pNode;
	}
}

template<class T>
typename EzDoublyLinkedList<T>::Node* EzDoublyLinkedList<T>::pop_front()
{
	if (NULL == m_pHead)
	{
		EzAssert(m_pTail == NULL);
		EzAssert(m_nSize == 0);
		return NULL;
	}
	else
	{
		Node* pNode = m_pHead;
		m_pHead = m_pHead->next;
		if (NULL == m_pHead)
		{
			m_pHead = m_pTail = NULL;
			EzAssert(m_nSize == 1);
			m_nSize = 0;
		}
		else
		{
			m_pHead->prev = NULL;
			--m_nSize;
		}

		pNode->next = pNode->prev = NULL;
		return pNode;
	}
}

template<class T>
void EzDoublyLinkedList<T>::insert(Iterator itr, Node* pNode)
{
	if (NULL == pNode)
		return;

	if (itr == head())
	{
		push_front(pNode);
		return;
	}
	if (itr == end())
	{
		push_back(pNode);
		return;
	}

	pNode->prev = itr->prev;
	itr->prev->next = pNode;
	pNode->next = itr.current();
	itr->prev = pNode;
	++m_nSize;
}

template<class T>
void EzDoublyLinkedList<T>::remove(Node* pNode)
{
	if (NULL == pNode)
		return;

	if (pNode == m_pHead)
	{
		pop_front();
		return;
	}
	if (pNode == m_pTail)
	{
		pop_back();
		return;
	}

	pNode->prev->next = pNode->next;
	pNode->next->prev = pNode->prev;
	--m_nSize;

	pNode->prev = pNode->next = NULL;
}

template<class T>
typename EzDoublyLinkedList<T>::Iterator EzDoublyLinkedList<T>::erase(Iterator itr)
{
	Iterator curItr = itr++;

	Node* pNode = curItr.current();
	remove(pNode);
	if (pNode)
		delete pNode;
	return itr;		// itr is already move to next node.
}

#endif // __XEZ_LINKED_LIST_H__