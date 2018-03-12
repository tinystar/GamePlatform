#include "MainUserManager.h"
#include "GameUserDefs.h"


MainUserManager::MainUserManager()
{

}

MainUserManager::~MainUserManager()
{
	cleanupAllUser();
}

bool MainUserManager::addLoginUser(ClientId cId, const UserInfo& userInfo)
{
	if (!EzVerify(cId.isValid() && userInfo.userId > 0))
		return false;

	if (!EzVerify(NULL == cId.getUserData()))
		return false;

	GameUser* pGameUser = new GameUser(userInfo);
	if (NULL == pGameUser)
		return false;

	cId.setUserData(pGameUser);

	EzAssert(m_userIdToClientMap.find(pGameUser->getUserId()) == m_userIdToClientMap.end());
	m_userIdToClientMap.insert(UserId2ClientIdMap::value_type(pGameUser->getUserId(), cId));
	return true;
}

bool MainUserManager::removeLoginUser(ClientId cId)
{
	void* pUserData = cId.getUserData();
	if (NULL == pUserData)
		return false;

	GameUser* pGameUser = (GameUser*)pUserData;

	cId.setUserData(NULL);
	UserId2ClientIdMap::iterator iter = m_userIdToClientMap.find(pGameUser->getUserId());
	EzAssert(iter != m_userIdToClientMap.end());
	if (iter != m_userIdToClientMap.end())
		m_userIdToClientMap.erase(iter);

	delete pGameUser;
	return true;
}

ClientId MainUserManager::findClientIdByUserId(EzUInt32 userId) const
{
	UserId2ClientIdMap::const_iterator iter = m_userIdToClientMap.find(userId);
	if (iter == m_userIdToClientMap.end())
		return ClientId::kNull;

	return iter->second;
}

GameUser* MainUserManager::getGameUserByClientId(ClientId cId) const
{
	void* pUserData = cId.getUserData();
	if (NULL == pUserData)
		return NULL;

	GameUser* pGameUser = (GameUser*)pUserData;
	return pGameUser;
}

void MainUserManager::cleanupAllUser()
{
	UserId2ClientIdMap::iterator iter = m_userIdToClientMap.begin();
	while (iter != m_userIdToClientMap.end())
	{
		ClientId id = iter->second;
		GameUser* pGameUser = (GameUser*)id.getUserData();
		id.setUserData(NULL);
		delete pGameUser;

		++iter;
	}

	m_userIdToClientMap.clear();
}
