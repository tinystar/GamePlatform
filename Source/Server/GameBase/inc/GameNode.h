/************************************************************************/
/* All rights reserved.
/* You can't use, copy or modify the file without the permission of the
/* author. Otherwise I will be held accountable under the law.
/*
/* Author: xiaoxin
/* Contact: 765392700(QQ)
/* Date: 2017/11/26
/************************************************************************/
#ifndef __GAME_NODE_H__
#define __GAME_NODE_H__

#include "xEzUtil.h"


class GameNode
{
public:
	enum NodeType
	{
		kTypeUnknown = 0,
		kTypeKind,
		kTypePlace,
		kTypeRoom
	};

	virtual ~GameNode();

public:
	NodeType type() const { return m_type; }

	GameNode* getParent() const { return m_parent; }

	int getChildCount() const { return m_children.logicalLength(); }

public:
	// pNode must be allocated on the heap
	virtual bool addChild(GameNode* pNode);
	// pNode will be deleted if it been removed from the list
	virtual bool deleteChild(GameNode* pNode);
	virtual void deleteAllChildren();
	virtual GameNode* getAt(int idx) { return m_children.at(idx); }

protected:
	GameNode();			// can't be instantiated

protected:
	NodeType			m_type;
	GameNode*			m_parent;
	EzArray<GameNode*>	m_children;
};

// a placeholder as the root node for contain GameKind nodes
class GameRoot : public GameNode
{
public:
#ifdef _DEBUG
	// for type-safe add in debug mode. Only GameKind type can be add to GameRoot.
	virtual bool addChild(GameNode* pNode);
#endif
};

class GameKind : public GameNode
{
public:
	GameKind();

#ifdef _DEBUG
	// for type-safe add in debug mode. Only GamePlace type node can be add to GameKind.
	virtual bool addChild(GameNode* pNode);
#endif
};

class GamePlace : public GameNode
{
public:
	GamePlace();

#ifdef _DEBUG
	// for type-safe add in debug mode. Only GameRoom type node can be add to GamePlace.
	virtual bool addChild(GameNode* pNode);
#endif
};

class GameRoom : public GameNode
{
public:
	GameRoom();
};

#endif // __GAME_NODE_H__
