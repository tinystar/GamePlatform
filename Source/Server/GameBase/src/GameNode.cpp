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
#endif


//-------------------------------------------------------------------------------
// GameRoom
//-------------------------------------------------------------------------------
GameRoom::GameRoom()
{
	m_type = kTypeRoom;
}
