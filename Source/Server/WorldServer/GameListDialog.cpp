#include "GameListDialog.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/document.h"
#include <ws2tcpip.h>
#include <afxdlgs.h>

using namespace rapidjson;

#define WORLD_CONFIG_FILE			_T("WorldConfig.json")
#define IDT_UPDATE_GAMEMGR			110

enum GameListColEnum
{
	eGLColGameId = 0,
	eGLColGameName,
	eGLColModName,
	eGLColLoadStatus,
	eGLColGameVer
};

enum RoomListColEnum
{
	eRLColGameId = 0,
	eRLColGameName,
	eRLColPlaceId,
	eRLColPlaceName,
	eRLColRoomId,
	eRLColRoomName,
	eRLColRunStatus,
	eRLColBasePoint,
	eRLColEnterLimit,
	eRLColServerAddr,
	eRLColServerPort,
	eRLColOnlineCount,
	eRLColMaxCount
};

struct ListColData
{
	UINT	uResId;
	int		nFormat;
	int		nWidth;
};

const static ListColData _s_gameListCol[] = {
	{ IDS_GAME_ID,		LVCFMT_LEFT, 50 },
	{ IDS_GAME_NAME,	LVCFMT_LEFT, 90 },
	{ IDS_MODULE_NAME,	LVCFMT_LEFT, 100 },
	{ IDS_LOAD_STATUS,	LVCFMT_LEFT, 60 },
	{ IDS_GAME_VERSION,	LVCFMT_LEFT, 60 }
};

const static ListColData _s_roomListCol[] = {
	{ IDS_GAME_ID,		LVCFMT_LEFT, 50 },
	{ IDS_GAME_NAME,	LVCFMT_LEFT, 90 },
	{ IDS_PLACE_ID,		LVCFMT_LEFT, 50 },
	{ IDS_PLACE_NAME,	LVCFMT_LEFT, 60 },
	{ IDS_ROOM_ID,		LVCFMT_LEFT, 50 },
	{ IDS_ROOM_NAME,	LVCFMT_LEFT, 60 },
	{ IDS_RUN_STATUS,	LVCFMT_LEFT, 60 },
	{ IDS_BASE_POINT,	LVCFMT_LEFT, 60 },
	{ IDS_ENTER_LIMIT,	LVCFMT_LEFT, 60 },
	{ IDS_ROOM_ADDRESS, LVCFMT_LEFT, 90 },
	{ IDS_ROOM_PORT,	LVCFMT_LEFT, 50 },
	{ IDS_ONLINE_COUNT, LVCFMT_LEFT, 60 },
	{ IDS_MAX_COUNT,	LVCFMT_LEFT, 60 }
};


BEGIN_MESSAGE_MAP(GameListDialog, CDialog)
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_LOAD_MODULE, &GameListDialog::OnBtnLoadModuleClick)
	ON_BN_CLICKED(IDC_UNLOAD_MODULE, &GameListDialog::OnBtnUnloadModuleClick)
	ON_BN_CLICKED(IDC_LOAD_ALL, &GameListDialog::OnBtnLoadAllClick)
	ON_BN_CLICKED(IDC_UNLOAD_ALL, &GameListDialog::OnBtnUnloadAllClick)
	ON_BN_CLICKED(IDC_UPDATE_MODULE, &GameListDialog::OnBtnUpdateModuleClick)
	ON_BN_CLICKED(IDC_START_ROOM, &GameListDialog::OnBtnStartRoomClick)
	ON_BN_CLICKED(IDC_STOP_ROOM, &GameListDialog::OnBtnStopRoomClick)
	ON_BN_CLICKED(IDC_START_ALL, &GameListDialog::OnBtnStartAllClick)
	ON_BN_CLICKED(IDC_STOP_ALL, &GameListDialog::OnBtnStopAllClick)
	ON_BN_CLICKED(IDC_UPDATE_LIST, &GameListDialog::OnBtnUpdateListClick)
END_MESSAGE_MAP()

GameListDialog::GameListDialog()
	: m_bInited(false)
{
	::memset(m_szHostIP, 0, sizeof(m_szHostIP));
	// test
	strcpy(m_szHostIP, "127.0.0.1");
}

void GameListDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GAME_LIST, m_gameList);
	DDX_Control(pDX, IDC_ROOM_LIST, m_roomList);
}

BOOL GameListDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_gameList.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	for (int i = 0; i < EzCountOf(_s_gameListCol); ++i)
	{
		CString sColHead;
		sColHead.LoadString(_s_gameListCol[i].uResId);
		m_gameList.InsertColumn(i, sColHead, _s_gameListCol[i].nFormat, _s_gameListCol[i].nWidth);
	}

	m_roomList.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	for (int i = 0; i < EzCountOf(_s_roomListCol); ++i)
	{
		CString sColHead;
		sColHead.LoadString(_s_roomListCol[i].uResId);
		m_roomList.InsertColumn(i, sColHead, _s_roomListCol[i].nFormat, _s_roomListCol[i].nWidth);
	}

	MgrInitConfig config;
	if (loadConfig(config))
	{
		if(GameServerMgr()->initialize(config))
		{
			m_bInited = true;
			GameServerMgr()->registerEventListener(this);
			GameServerMgr()->updateGameList();
			SetTimer(IDT_UPDATE_GAMEMGR, 20, NULL);
		}
		else
		{
			MessageBox(_T("Init GameServerManager failed!"));
		}
	}

	return TRUE;
}

void GameListDialog::OnDestroy()
{
	CDialog::OnDestroy();

	if (m_bInited)
	{
		KillTimer(IDT_UPDATE_GAMEMGR);
		GameServerMgr()->registerEventListener(NULL);
		GameServerMgr()->unInitialize();
		m_bInited = false;
	}

	clearGameListCtrl();
	clearRoomListCtrl();
	GameServerManager::destroyInstance();
}

void GameListDialog::OnTimer(UINT_PTR nIDEvent)
{
	if (IDT_UPDATE_GAMEMGR == nIDEvent)
	{
		GameServerMgr()->doUpdate();
		return;
	}

	CDialog::OnTimer(nIDEvent);
}

void GameListDialog::OnBtnLoadModuleClick()
{
	POSITION pos = m_gameList.GetFirstSelectedItemPosition();
	if (NULL == pos)
	{
		CString sTip;
		sTip.LoadString(IDS_SEL_LOADMODULE_TIP);
		MessageBox(sTip);
		return;
	}

	while (pos)
	{
		int nItem = m_gameList.GetNextSelectedItem(pos);
		loadModuleAtItem(nItem);
	}
}

void GameListDialog::OnBtnUnloadModuleClick()
{
	POSITION pos = m_gameList.GetFirstSelectedItemPosition();
	if (NULL == pos)
	{
		CString sTip;
		sTip.LoadString(IDS_SEL_UNLOADMODULE_TIP);
		MessageBox(sTip);
		return;
	}

	while (pos)
	{
		int nItem = m_gameList.GetNextSelectedItem(pos);
		unloadModuleAtItem(nItem);
	}
}

void GameListDialog::OnBtnLoadAllClick()
{
	for (int i = 0; i < m_gameList.GetItemCount(); ++i)
		loadModuleAtItem(i);
}

void GameListDialog::OnBtnUnloadAllClick()
{
	for (int i = 0; i < m_gameList.GetItemCount(); ++i)
		unloadModuleAtItem(i);
}

void GameListDialog::OnBtnUpdateModuleClick()
{
	if (m_gameList.GetSelectedCount() != 1)
	{
		CString sTip;
		sTip.LoadString(IDS_ONE_UPD_MODULE_TIP);
		MessageBox(sTip);
		return;
	}

	int nItem = m_gameList.GetSelectionMark();
	GameKind* pGameKind = (GameKind*)m_gameList.GetItemData(nItem);
	EzAssert(pGameKind != NULL);

	CFileDialog dlgFile(TRUE, NULL, NULL, 0, _T("Game Module File(*.dll)|*.dll||"));
	if (IDOK == dlgFile.DoModal())
	{
		CString sFullPath = dlgFile.GetPathName();
		pGameKind->m_kindInfo.sSvrModFullPath = EzString(sFullPath.GetString()).kcharPtr(kAnsi);
		m_gameList.SetItemText(nItem, eGLColModName, dlgFile.GetFileName());

		unloadModuleAtItem(nItem);
		loadModuleAtItem(nItem);
	}
}

void GameListDialog::OnBtnStartRoomClick()
{
	POSITION pos = m_roomList.GetFirstSelectedItemPosition();
	if (NULL == pos)
	{
		CString sTip;
		sTip.LoadString(IDS_SEL_STARTROOM_TIP);
		MessageBox(sTip);
		return;
	}

	while (pos)
	{
		int nItem = m_roomList.GetNextSelectedItem(pos);
		startRoomAtItem(nItem);
	}
}

void GameListDialog::OnBtnStopRoomClick()
{
	POSITION pos = m_roomList.GetFirstSelectedItemPosition();
	if (NULL == pos)
	{
		CString sTip;
		sTip.LoadString(IDS_SEL_STOPROOM_TIP);
		MessageBox(sTip);
		return;
	}

	while (pos)
	{
		int nItem = m_roomList.GetNextSelectedItem(pos);
		stopRoomAtItem(nItem);
	}
}

void GameListDialog::OnBtnStartAllClick()
{
	for (int i = 0; i < m_roomList.GetItemCount(); ++i)
		startRoomAtItem(i);
}

void GameListDialog::OnBtnStopAllClick()
{
	for (int i = 0; i < m_roomList.GetItemCount(); ++i)
		stopRoomAtItem(i);
}

void GameListDialog::OnBtnUpdateListClick()
{
	clearGameListCtrl();
	clearRoomListCtrl();
	GameServerMgr()->updateGameList();
}

bool GameListDialog::loadConfig(MgrInitConfig& config)
{
	TCHAR szPath[MAX_PATH] = { 0 };
	bool bRet = EzSys::getCurModulePath(szPath, MAX_PATH);
	if (!bRet)
		return false;

	size_t nLen = _tcslen(szPath);
	nLen += _tcslen(WORLD_CONFIG_FILE);
	if (nLen >= MAX_PATH)
		return false;

	_tcscat_s(szPath, MAX_PATH, WORLD_CONFIG_FILE);

	FILE* fp = _tfopen(szPath, _T("r"));
	if (NULL == fp)
		return false;

	char readBuf[8192];
	FileReadStream is(fp, readBuf, sizeof(readBuf));
	fclose(fp);

	Document doc;
	doc.ParseStream(is);

	config.bDebugMode = doc["debugMode"].GetBool();
	LogLevel logLevel = (LogLevel)doc["logLevel"].GetInt();
	config.nRoomPkgSize = (LogLevel)doc["roomPkgSize"].GetUint();
	const Value& DBSvrCfg = doc["DBSvrAddr"];
	const char* pszDBAddr = DBSvrCfg["addr"].GetString();
	unsigned short sDBPort = (unsigned short)DBSvrCfg["port"].GetInt();

	EzLogger::setLogLevel(logLevel);

	strncpy(config.szDBAddress, pszDBAddr, 19);
	config.sDBPort = sDBPort;

	return true;
}

void GameListDialog::updateListCtrl()
{
	const GameListTree& gameList = GameServerMgr()->getGameList();

	for (int i = 0; i < gameList.getGameCount(); ++i)
	{
		GameKind* pKind = gameList.gameKindAt(i);
		if (NULL == pKind)
			continue;

		m_gameList.InsertItem(i, NULL);
		m_gameList.SetItemData(i, (DWORD_PTR)pKind);

		TCHAR szBuf[256] = { 0 };
		_stprintf_s(szBuf, 256, _T("%d"), pKind->m_kindInfo.nKindId);
		CString sStatus;
		sStatus.LoadString(IDS_NOT_LOADED);
		m_gameList.SetItemText(i, 0, szBuf);
		m_gameList.SetItemText(i, 1, EzString(pKind->m_kindInfo.szGameName, kUtf8).kwcharPtr());
		m_gameList.SetItemText(i, 2, EzString(pKind->m_kindInfo.szServerModule, kUtf8).kwcharPtr());
		m_gameList.SetItemText(i, 3, sStatus);
		m_gameList.SetItemText(i, 4, EzString(pKind->m_kindInfo.szVersion, kUtf8).kwcharPtr());

		for (int j = 0; j < pKind->getChildCount(); ++j)
		{
			GameNode* pNode = pKind->getAt(j);
			if (NULL == pNode || GameNode::kTypePlace != pNode->type())
				continue;

			GamePlace* pPlace = (GamePlace*)pNode;
			for (int k = 0; k < pPlace->getChildCount(); ++k)
			{
				pNode = pPlace->getAt(k);
				if (NULL == pNode || GameNode::kTypeRoom != pNode->type())
					continue;

				GameRoom* pRoom = (GameRoom*)pNode;
				if (!isIPAddressEqual(pRoom->m_roomInfo.szServerIp, m_szHostIP))
					continue;

				int idx = m_roomList.GetItemCount();
				idx = m_roomList.InsertItem(idx, NULL);
				m_roomList.SetItemData(idx, (DWORD_PTR)pRoom);

				_stprintf_s(szBuf, 256, _T("%d"), pKind->m_kindInfo.nKindId);
				m_roomList.SetItemText(idx, 0, szBuf);
				m_roomList.SetItemText(idx, 1, EzString(pKind->m_kindInfo.szGameName, kUtf8).kwcharPtr());
				_stprintf_s(szBuf, 256, _T("%d"), pPlace->m_placeInfo.nPlaceId);
				m_roomList.SetItemText(idx, 2, szBuf);
				m_roomList.SetItemText(idx, 3, EzString(pPlace->m_placeInfo.szPlaceName, kUtf8).kwcharPtr());
				_stprintf_s(szBuf, 256, _T("%d"), pRoom->m_roomInfo.nRoomId);
				m_roomList.SetItemText(idx, 4, szBuf);
				m_roomList.SetItemText(idx, 5, EzString(pRoom->m_roomInfo.szRoomName, kUtf8).kwcharPtr());
				sStatus.LoadString(IDS_NOT_RUNNING);
				m_roomList.SetItemText(idx, 6, sStatus);
				_stprintf_s(szBuf, 256, _T("%.2f"), pPlace->m_placeInfo.dBasePoint);
				m_roomList.SetItemText(idx, 7, szBuf);
				_stprintf_s(szBuf, 256, _T("%.2f"), pPlace->m_placeInfo.dEnterLimit);
				m_roomList.SetItemText(idx, 8, szBuf);
				m_roomList.SetItemText(idx, 9, EzString(pRoom->m_roomInfo.szServerIp, kUtf8).kwcharPtr());
				_stprintf_s(szBuf, 256, _T("%d"), pRoom->m_roomInfo.sServerPort);
				m_roomList.SetItemText(idx, 10, szBuf);
				_stprintf_s(szBuf, 256, _T("%d"), pRoom->m_roomInfo.uOnlineCount);
				m_roomList.SetItemText(idx, 11, szBuf);
				_stprintf_s(szBuf, 256, _T("%d"), pRoom->m_roomInfo.uMaxUserCount);
				m_roomList.SetItemText(idx, 12, szBuf);
			}
		}
	}
}

bool GameListDialog::isIPAddressEqual(const char* pszIP1, const char* pszIP2)
{
	if (NULL == pszIP1 && NULL == pszIP2)
		return true;
	if (NULL == pszIP1 || NULL == pszIP2)
		return false;

	struct in_addr s1, s2;
	inet_pton(AF_INET, pszIP1, (void*)&s1);
	inet_pton(AF_INET, pszIP2, (void*)&s2);

	return s1.s_addr == s2.s_addr;
}

void GameListDialog::loadModuleAtItem(int nItem)
{
	if (0 <= nItem && nItem < m_gameList.GetItemCount())
	{
		GameKind* pGameKind = (GameKind*)m_gameList.GetItemData(nItem);
		EzAssert(pGameKind != NULL);

		if (GameServerMgr()->loadGameModule(pGameKind))
		{
			CString sStatus;
			sStatus.LoadString(IDS_HAS_LOADED);
			m_gameList.SetItemText(nItem, eGLColLoadStatus, sStatus);
		}
	}
}

void GameListDialog::unloadModuleAtItem(int nItem)
{
	if (0 <= nItem && nItem < m_gameList.GetItemCount())
	{
		GameKind* pGameKind = (GameKind*)m_gameList.GetItemData(nItem);
		EzAssert(pGameKind != NULL);

		if (GameServerMgr()->unloadGameModule(pGameKind))
		{
			CString sStatus;
			sStatus.LoadString(IDS_NOT_LOADED);
			m_gameList.SetItemText(nItem, eGLColLoadStatus, sStatus);
		}
	}
}

void GameListDialog::startRoomAtItem(int nItem)
{
	if (0 <= nItem && nItem < m_gameList.GetItemCount())
	{
		GameRoom* pGameRoom = (GameRoom*)m_roomList.GetItemData(nItem);
		EzAssert(pGameRoom != NULL);

		if (GameServerMgr()->startGameRoom(pGameRoom))
		{
			CString sStatus;
			sStatus.LoadString(IDS_IS_RUNNING);
			m_roomList.SetItemText(nItem, eRLColRunStatus, sStatus);
		}
	}
}

void GameListDialog::stopRoomAtItem(int nItem)
{
	if (0 <= nItem && nItem < m_gameList.GetItemCount())
	{
		GameRoom* pGameRoom = (GameRoom*)m_roomList.GetItemData(nItem);
		EzAssert(pGameRoom != NULL);

		if (GameServerMgr()->stopGameRoom(pGameRoom))
		{
			CString sStatus;
			sStatus.LoadString(IDS_NOT_RUNNING);
			m_roomList.SetItemText(nItem, eRLColRunStatus, sStatus);
		}
	}
}

void GameListDialog::onUpdateGameListOver(void)
{
	updateListCtrl();
}

void GameListDialog::clearGameListCtrl()
{
	for (int i = 0; i < m_gameList.GetItemCount(); ++i)
	{
		GameKind* pGameKind = (GameKind*)m_gameList.GetItemData(i);
		EzAssert(pGameKind != NULL);

		GameServerMgr()->cleanupGameModule(pGameKind);
		m_gameList.SetItemData(i, NULL);
	}

	m_gameList.DeleteAllItems();
}

void GameListDialog::clearRoomListCtrl()
{
	for (int i = 0; i < m_roomList.GetItemCount(); ++i)
	{
		GameRoom* pGameRoom = (GameRoom*)m_roomList.GetItemData(i);
		EzAssert(pGameRoom != NULL);

		GameServerMgr()->cleanupGameRoom(pGameRoom);
		m_roomList.SetItemData(i, NULL);
	}

	m_roomList.DeleteAllItems();
}
