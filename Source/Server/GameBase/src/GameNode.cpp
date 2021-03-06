#include "GameNode.h"


//-------------------------------------------------------------------------------
// GameNode
//-------------------------------------------------------------------------------
GameNode::GameNode()
	: m_type(kTypeUnknown)
	, m_parent(NULL)
	, m_pData(NULL)
{

}

GameNode::~GameNode()
{
	deleteAllChildren();
	m_pData = NULL;
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


//-------------------------------------------------------------------------------
// GameRoom
//-------------------------------------------------------------------------------
GameRoom::GameRoom()
{
	m_type = kTypeRoom;
}


//-------------------------------------------------------------------------------
// GameListTree
//-------------------------------------------------------------------------------
GameKind* GameListTree::gameKindAt(int idx) const
{
	GameNode* pNode = m_rootNode.getAt(idx);
	if (pNode != NULL && GameNode::kTypeKind == pNode->type())
		return (GameKind*)pNode;

	return NULL;
}

bool GameListTree::addGameKind(GameKind* pKind)
{
	return m_rootNode.addChild(pKind);
}

bool GameListTree::addGamePlace(EzInt32 nKindId, GamePlace* pPlace)
{
	GameKind* pKind = findGameKindById(nKindId);
	if (pKind != NULL)
		return pKind->addChild(pPlace);

	return false;
}

bool GameListTree::addGameRoom(EzInt32 nKindId, EzInt32 nPlaceId, GameRoom* pRoom)
{
	GamePlace* pPlace = findGamePlaceById(nKindId, nPlaceId);
	if (pPlace != NULL)
		return pPlace->addChild(pRoom);

	return false;
}

GameKind* GameListTree::findGameKindById(EzInt32 nKindId) const
{
	for (int i = 0; i < m_rootNode.getChildCount(); ++i)
	{
		GameNode* pNode = m_rootNode.getAt(i);
		if (pNode != NULL && GameNode::kTypeKind == pNode->type())
		{
			GameKind* pKind = (GameKind*)pNode;
			if (pKind->m_kindInfo.nKindId == nKindId)
				return pKind;
		}
	}

	return NULL;
}

GamePlace* GameListTree::findGamePlaceById(EzInt32 nKindId, EzInt32 nPlaceId) const
{
	GameKind* pKind = findGameKindById(nKindId);
	if (pKind != NULL)
	{
		for (int i = 0; i < pKind->getChildCount(); ++i)
		{
			GameNode* pNode = pKind->getAt(i);
			if (pNode != NULL && GameNode::kTypePlace == pNode->type())
			{
				GamePlace* pPlace = (GamePlace*)pNode;
				if (pPlace->m_placeInfo.nPlaceId == nPlaceId)
					return pPlace;
			}
		}
	}

	return NULL;
}

GameRoom* GameListTree::findGameRoomById(EzInt32 nKindId, EzInt32 nPlaceId, EzInt32 nRoomId) const
{
	GamePlace* pPlace = findGamePlaceById(nKindId, nPlaceId);
	if (pPlace != NULL)
	{
		for (int i = 0; i < pPlace->getChildCount(); ++i)
		{
			GameNode* pNode = pPlace->getAt(i);
			if (pNode != NULL && GameNode::kTypeRoom == pNode->type())
			{
				GameRoom* pRoom = (GameRoom*)pNode;
				if (pRoom->m_roomInfo.nRoomId == nRoomId)
					return pRoom;
			}
		}
	}

	return NULL;
}


//-------------------------------------------------------------------------------
// GameRoomIterator
//-------------------------------------------------------------------------------
GameRoomIterator::GameRoomIterator(GameNode* pRoot)
	: m_pRootNode(pRoot)
	, m_pCurRoom(NULL)
{
}

void GameRoomIterator::start()
{
	if (NULL == m_pRootNode)
		return;

	// clear path stack
	while (!m_iterPath.empty())
		m_iterPath.pop();

	m_pCurRoom = NULL;

	m_iterPath.push(NodeIterState(m_pRootNode));
	next();
}

void GameRoomIterator::next()
{
	while (!m_iterPath.empty())
	{
		NodeIterState& nod = m_iterPath.top();		// note!!! reference
		EzAssert(nod.pNode != NULL);
		if (GameNode::kTypeRoom == nod.pNode->type())
		{
			m_pCurRoom = (GameRoom*)nod.pNode;
			m_iterPath.pop();
			return;
		}
		else
		{
			if (nod.nIterIdx >= nod.pNode->getChildCount())
			{
				m_iterPath.pop();
			}
			else
			{
				GameNode* pChild = nod.pNode->getAt(nod.nIterIdx);
				if (pChild != NULL)
					m_iterPath.push(NodeIterState(pChild));

				++nod.nIterIdx;
			}
		}
	}

	m_pCurRoom = NULL;
}

bool GameRoomIterator::done()
{
	return m_pCurRoom == NULL;
}
