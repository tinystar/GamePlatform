#include "GameNode.h"


//-------------------------------------------------------------------------------
// GameNode
//-------------------------------------------------------------------------------
GameNode::GameNode()
	: m_type(kTypeUnknown)
	, m_parent(NULL)
{

}

GameNode::~GameNode()
{
	deleteAllChildren();
}

bool GameNode::addChild(GameNode* pNode)
{
	if (!EzVerify(pNode != NULL))
		return false;

	if (!EzVerify(pNode->m_parent == NULL))
		return false;

	if (!EzVerify(pNode->m_type != kTypeUnknown))
		return false;

	m_children.append(pNode);
	pNode->m_parent = this;
	return true;
}

bool GameNode::insertChild(int idx, GameNode* pNode)
{
	if (!EzVerify(pNode != NULL))
		return false;

	if (!EzVerify(pNode->m_parent == NULL))
		return false;

	if (!EzVerify(pNode->m_type != kTypeUnknown))
		return false;

	if (!EzVerify(0 <= idx && idx <= m_children.logicalLength()))
		return false;

	m_children.insertAt(idx, pNode);
	pNode->m_parent = this;
	return true;
}

bool GameNode::deleteChild(GameNode* pNode)
{
	if (NULL == pNode)
		return false;

	bool bRet = m_children.remove(pNode);
	if (bRet)
		delete pNode;

	return bRet;
}

void GameNode::deleteAllChildren()
{
	for (int i = 0; i < m_children.logicalLength(); ++i)
		delete m_children.at(i);

	m_children.removeAll();
}


//-------------------------------------------------------------------------------
// GameRoot
//-------------------------------------------------------------------------------
#ifdef _DEBUG
bool GameRoot::addChild(GameNode* pNode)
{
	if (!EzVerify(pNode != NULL))
		return false;

	if (!EzVerify(pNode->type() == kTypeKind))
		return false;

	return GameNode::addChild(pNode);
}

bool GameRoot::insertChild(int idx, GameNode* pNode)
{
	if (!EzVerify(pNode != NULL))
		return false;

	if (!EzVerify(pNode->type() == kTypeKind))
		return false;

	return GameNode::insertChild(idx, pNode);
}
#endif

GameNode* GameRoot::findChildById(EzInt32 nodId)
{
	for (int i = 0; i < m_children.logicalLength(); ++i)
	{
		if (EzVerify(kTypeKind == m_children[i]->type()))
		{
			GameKind* pKind = (GameKind*)m_children[i];
			if (pKind->m_kindInfo.nKindId == nodId)
				return pKind;
		}
	}

	return NULL;
}


//-------------------------------------------------------------------------------
// GameKind
//-------------------------------------------------------------------------------
GameKind::GameKind()
{
	m_type = kTypeKind;
}

#ifdef _DEBUG
bool GameKind::addChild(GameNode* pNode)
{
	if (!EzVerify(pNode != NULL))
		return false;

	if (!EzVerify(pNode->type() == kTypePlace))
		return false;

	return GameNode::addChild(pNode);
}

bool GameKind::insertChild(int idx, GameNode* pNode)
{
	if (!EzVerify(pNode != NULL))
		return false;

	if (!EzVerify(pNode->type() == kTypePlace))
		return false;

	return GameNode::insertChild(idx, pNode);
}
#endif

GameNode* GameKind::findChildById(EzInt32 nodId)
{
	for (int i = 0; i < m_children.logicalLength(); ++i)
	{
		if (EzVerify(kTypePlace == m_children[i]->type()))
		{
			GamePlace* pPlace = (GamePlace*)m_children[i];
			if (pPlace->m_placeInfo.nPlaceId == nodId)
				return pPlace;
		}
	}

	return NULL;
}


//-------------------------------------------------------------------------------
// GamePlace
//-------------------------------------------------------------------------------
GamePlace::GamePlace()
{
	m_type = kTypePlace;
}

#ifdef _DEBUG
bool GamePlace::addChild(GameNode* pNode)
{
	if (!EzVerify(pNode != NULL))
		return false;

	if (!EzVerify(pNode->type() == kTypeRoom))
		return false;

	return GameNode::addChild(pNode);
}

bool GamePlace::insertChild(int idx, GameNode* pNode)
{
	if (!EzVerify(pNode != NULL))
		return false;

	if (!EzVerify(pNode->type() == kTypeRoom))
		return false;

	return GameNode::insertChild(idx, pNode);
}
#endif

GameNode* GamePlace::findChildById(EzInt32 nodId)
{
	for (int i = 0; i < m_children.logicalLength(); ++i)
	{
		if (EzVerify(kTypeRoom == m_children[i]->type()))
		{
			GameRoom* pRoom = (GameRoom*)m_children[i];
			if (pRoom->m_roomInfo.nRoomId == nodId)
				return pRoom;
		}
	}

	return NULL;
}


//-------------------------------------------------------------------------------
// GameRoom
//-------------------------------------------------------------------------------
GameRoom::GameRoom()
{
	m_type = kTypeRoom;
}
