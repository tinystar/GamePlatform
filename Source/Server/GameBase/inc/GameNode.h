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
#include "GameNodeInfo.h"
#include "GameBase.h"

class GameNode;

template class GB_DLL_SPEC EzArray < GameNode* >;	// avoid warning C4251
class GB_DLL_SPEC GameNode
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
	virtual bool insertChild(int idx, GameNode* pNode);
	// pNode will be deleted if it been removed from the list
	virtual bool deleteChild(GameNode* pNode);
	virtual void deleteAllChildren();

	virtual GameNode* getAt(int idx) const { return m_children.at(idx); }

protected:
	GameNode();			// can't be instantiated

protected:
	NodeType			m_type;
	GameNode*			m_parent;
	EzArray<GameNode*>	m_children;
};

// a placeholder as the root node for contain GameKind nodes
class GB_DLL_SPEC GameRoot : public GameNode
{
public:
#ifdef _DEBUG
	// for type-safe add in debug mode. Only GameKind type can be add to GameRoot.
	virtual bool addChild(GameNode* pNode);
	virtual bool insertChild(int idx, GameNode* pNode);
#endif
};

class GB_DLL_SPEC GameKind : public GameNode
{
public:
	GameKind();

#ifdef _DEBUG
	// for type-safe add in debug mode. Only GamePlace type node can be add to GameKind.
	virtual bool addChild(GameNode* pNode);
	virtual bool insertChild(int idx, GameNode* pNode);
#endif

public:
	GameKindInfo	m_kindInfo;
};

class GB_DLL_SPEC GamePlace : public GameNode
{
public:
	GamePlace();

#ifdef _DEBUG
	// for type-safe add in debug mode. Only GameRoom type node can be add to GamePlace.
	virtual bool addChild(GameNode* pNode);
	virtual bool insertChild(int idx, GameNode* pNode);
#endif

public:
	GamePlaceInfo	m_placeInfo;
};

class GB_DLL_SPEC GameRoom : public GameNode
{
public:
	GameRoom();

public:
	GameRoomInfo	m_roomInfo;
};

class GB_DLL_SPEC GameListTree
{
public:
	int getGameCount() const { return m_rootNode.getChildCount(); }
	bool isEmpty() const { return 0 == getGameCount(); }

	bool addGameKind(GameKind* pKind);
	bool addGamePlace(EzInt32 nKindId, GamePlace* pPlace);
	bool addGameRoom(EzInt32 nKindId, EzInt32 nPlaceId, GameRoom* pRoom);

	GameKind* findGameKindById(EzInt32 nKindId) const;
	GamePlace* findGamePlaceById(EzInt32 nKindId, EzInt32 nPlaceId) const;
	GameRoom* findGameRoomById(EzInt32 nKindId, EzInt32 nPlaceId, EzInt32 nRoomId) const;

protected:
	GameRoot	m_rootNode;
};

#endif // __GAME_NODE_H__
