#include "GameListDialog.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/document.h"

using namespace rapidjson;

#define WORLD_CONFIG_FILE			_T("WorldConfig.json")
#define IDT_UPDATE_GAMEMGR			110
#define IDT_UPDATE_LIST				111

BEGIN_MESSAGE_MAP(GameListDialog, CDialog)
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_LOAD_MODULE, &GameListDialog::OnBtnLoadModuleClick)
	ON_BN_CLICKED(IDC_UNLOAD_MODULE, &GameListDialog::OnBtnUnloadModuleClick)
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
	m_gameList.InsertColumn(0, _T("游戏Id"), LVCFMT_LEFT, 50);
	m_gameList.InsertColumn(1, _T("游戏名称"), LVCFMT_LEFT, 90);
	m_gameList.InsertColumn(2, _T("模块名称"), LVCFMT_LEFT, 100);
	m_gameList.InsertColumn(3, _T("加载状态"), LVCFMT_LEFT, 60);
	m_gameList.InsertColumn(4, _T("游戏版本"), LVCFMT_LEFT, 60);

	m_roomList.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	m_roomList.InsertColumn(0, _T("游戏Id"), LVCFMT_LEFT, 50);
	m_roomList.InsertColumn(1, _T("游戏名称"), LVCFMT_LEFT, 90);
	m_roomList.InsertColumn(2, _T("场次Id"), LVCFMT_LEFT, 50);
	m_roomList.InsertColumn(3, _T("场次名称"), LVCFMT_LEFT, 60);
	m_roomList.InsertColumn(4, _T("房间Id"), LVCFMT_LEFT, 50);
	m_roomList.InsertColumn(5, _T("房间名称"), LVCFMT_LEFT, 60);
	m_roomList.InsertColumn(6, _T("启动状态"), LVCFMT_LEFT, 60);
	m_roomList.InsertColumn(7, _T("底分"), LVCFMT_LEFT, 50);
	m_roomList.InsertColumn(8, _T("进入限制"), LVCFMT_LEFT, 60);
	m_roomList.InsertColumn(9, _T("地址"), LVCFMT_LEFT, 90);
	m_roomList.InsertColumn(10, _T("端口"), LVCFMT_LEFT, 50);
	m_roomList.InsertColumn(11, _T("在线人数"), LVCFMT_LEFT, 60);
	m_roomList.InsertColumn(12, _T("最大人数"), LVCFMT_LEFT, 60);

	MgrInitConfig config;
	if (loadConfig(config))
	{
		if(GameServerMgr()->initialize(config))
		{
			m_bInited = true;
			GameServerMgr()->updateGameList();
			SetTimer(IDT_UPDATE_GAMEMGR, 20, NULL);
			SetTimer(IDT_UPDATE_LIST, 1000, NULL);
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
		GameServerMgr()->unInitialize();
		m_bInited = false;
	}
	GameServerManager::destroyInstance();
}

void GameListDialog::OnTimer(UINT_PTR nIDEvent)
{
	if (IDT_UPDATE_GAMEMGR == nIDEvent)
	{
		GameServerMgr()->doUpdate();
		return;
	}
	else if (IDT_UPDATE_LIST == nIDEvent)
	{
		updateListCtrl();
		KillTimer(IDT_UPDATE_LIST);
		return;
	}

	CDialog::OnTimer(nIDEvent);
}

void GameListDialog::OnBtnLoadModuleClick()
{

}

void GameListDialog::OnBtnUnloadModuleClick()
{

}

void GameListDialog::OnBtnLoadAllClick()
{

}

void GameListDialog::OnBtnUnloadAllClick()
{

}

void GameListDialog::OnBtnUpdateModuleClick()
{

}

void GameListDialog::OnBtnStartRoomClick()
{

}

void GameListDialog::OnBtnStopRoomClick()
{

}

void GameListDialog::OnBtnStartAllClick()
{

}

void GameListDialog::OnBtnStopAllClick()
{

}

void GameListDialog::OnBtnUpdateListClick()
{

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
		m_gameList.SetItemText(i, 0, szBuf);
		m_gameList.SetItemText(i, 1, EzString(pKind->m_kindInfo.szGameName, kUtf8).kwcharPtr());
		m_gameList.SetItemText(i, 2, EzString(pKind->m_kindInfo.szServerModule, kUtf8).kwcharPtr());
		m_gameList.SetItemText(i, 3, _T("未加载"));
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
				m_roomList.SetItemText(idx, 6, _T("未启动"));
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
	// Todo
	return true;
}
