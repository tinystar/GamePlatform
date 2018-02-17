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

	m_gameList.InsertColumn(0, _T("��ϷId"), LVCFMT_LEFT, 50);
	m_gameList.InsertColumn(1, _T("��Ϸ����"), LVCFMT_LEFT, 100);
	m_gameList.InsertColumn(2, _T("ģ������"), LVCFMT_LEFT, 100);
	m_gameList.InsertColumn(3, _T("����״̬"), LVCFMT_LEFT, 80);
	m_gameList.InsertColumn(4, _T("��Ϸ�汾"), LVCFMT_LEFT, 60);

	m_roomList.InsertColumn(0, _T("��ϷId"), LVCFMT_LEFT, 50);
	m_roomList.InsertColumn(1, _T("��Ϸ����"), LVCFMT_LEFT, 100);
	m_roomList.InsertColumn(2, _T("����Id"), LVCFMT_LEFT, 50);
	m_roomList.InsertColumn(3, _T("��������"), LVCFMT_LEFT, 60);
	m_roomList.InsertColumn(4, _T("����Id"), LVCFMT_LEFT, 50);
	m_roomList.InsertColumn(5, _T("��������"), LVCFMT_LEFT, 60);
	m_roomList.InsertColumn(6, _T("�׷�"), LVCFMT_LEFT, 50);
	m_roomList.InsertColumn(7, _T("��������"), LVCFMT_LEFT, 60);
	m_roomList.InsertColumn(8, _T("��ַ"), LVCFMT_LEFT, 100);
	m_roomList.InsertColumn(9, _T("�˿�"), LVCFMT_LEFT, 50);
	m_roomList.InsertColumn(10, _T("��������"), LVCFMT_LEFT, 60);
	m_roomList.InsertColumn(11, _T("�������"), LVCFMT_LEFT, 60);

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
