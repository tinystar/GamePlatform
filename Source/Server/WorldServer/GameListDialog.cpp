#include "GameListDialog.h"


BEGIN_MESSAGE_MAP(GameListDialog, CDialog)
	ON_BN_CLICKED(IDC_LOAD_MODULE, &GameListDialog::OnBtnLoadModuleClick)
	ON_BN_CLICKED(IDC_UNLOAD_MODULE, &GameListDialog::OnBtnUnloadModuleClick)
	ON_BN_CLICKED(IDC_UPDATE_MODULE, &GameListDialog::OnBtnUpdateModuleClick)
	ON_BN_CLICKED(IDC_UPDATE_MODULE, &GameListDialog::OnBtnStartRoomClick)
	ON_BN_CLICKED(IDC_UPDATE_MODULE, &GameListDialog::OnBtnStopRoomClick)
	ON_BN_CLICKED(IDC_UPDATE_MODULE, &GameListDialog::OnBtnStartAllClick)
	ON_BN_CLICKED(IDC_UPDATE_MODULE, &GameListDialog::OnBtnStopAllClick)
	ON_BN_CLICKED(IDC_UPDATE_MODULE, &GameListDialog::OnBtnUpdateListClick)
END_MESSAGE_MAP()

GameListDialog::GameListDialog()
{

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

	m_gameList.InsertColumn(0, _T("游戏Id"), LVCFMT_LEFT, 50);
	m_gameList.InsertColumn(1, _T("游戏名称"), LVCFMT_LEFT, 100);
	m_gameList.InsertColumn(2, _T("模块名称"), LVCFMT_LEFT, 100);
	m_gameList.InsertColumn(3, _T("加载状态"), LVCFMT_LEFT, 80);
	m_gameList.InsertColumn(4, _T("游戏版本"), LVCFMT_LEFT, 60);

	m_roomList.InsertColumn(0, _T("游戏Id"), LVCFMT_LEFT, 50);
	m_roomList.InsertColumn(1, _T("游戏名称"), LVCFMT_LEFT, 100);
	m_roomList.InsertColumn(2, _T("场次Id"), LVCFMT_LEFT, 50);
	m_roomList.InsertColumn(3, _T("场次名称"), LVCFMT_LEFT, 60);
	m_roomList.InsertColumn(4, _T("房间Id"), LVCFMT_LEFT, 50);
	m_roomList.InsertColumn(5, _T("房间名称"), LVCFMT_LEFT, 60);
	m_roomList.InsertColumn(6, _T("底分"), LVCFMT_LEFT, 50);
	m_roomList.InsertColumn(7, _T("进入限制"), LVCFMT_LEFT, 60);
	m_roomList.InsertColumn(8, _T("地址"), LVCFMT_LEFT, 100);
	m_roomList.InsertColumn(9, _T("端口"), LVCFMT_LEFT, 50);
	m_roomList.InsertColumn(10, _T("在线人数"), LVCFMT_LEFT, 60);
	m_roomList.InsertColumn(11, _T("最大人数"), LVCFMT_LEFT, 60);

	return TRUE;
}

void GameListDialog::OnBtnLoadModuleClick()
{

}

void GameListDialog::OnBtnUnloadModuleClick()
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
