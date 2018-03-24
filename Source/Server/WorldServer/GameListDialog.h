/************************************************************************/
/* All rights reserved.
/* You can't use, copy or modify the file without the permission of the
/* author. Otherwise I will be held accountable under the law.
/*
/* Author: xiaoxin
/* Contact: 765392700(QQ)
/* Date: 2018/2/15
/************************************************************************/
#ifndef __GAMELIST_DIALOG_H__
#define __GAMELIST_DIALOG_H__

#include <afxwin.h>
#include <afxcmn.h>
#include "Resource.h"
#include "GameServerManager.h"

class GameListDialog : public CDialog, public IGameServerMgrEventListener
{
public:
	GameListDialog();

	enum { IDD = IDD_GAME_LIST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

	virtual BOOL OnInitDialog();

protected:
	virtual void onUpdateGameListOver(void);

protected:
	bool loadConfig(MgrInitConfig& config);

	void updateListCtrl();

	bool isIPAddressEqual(const char* pszIP1, const char* pszIP2);

	void loadModuleAtItem(int nItem);
	void unloadModuleAtItem(int nItem);

	void startRoomAtItem(int nItem);
	void stopRoomAtItem(int nItem);

	void clearGameListCtrl();
	void clearRoomListCtrl();

	void updateRoomItemRunStatus(int item, bool bStopped);

protected:
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	afx_msg void OnBtnLoadModuleClick();
	afx_msg void OnBtnUnloadModuleClick();
	afx_msg void OnBtnLoadAllClick();
	afx_msg void OnBtnUnloadAllClick();
	afx_msg void OnBtnUpdateModuleClick();

	afx_msg void OnBtnStartRoomClick();
	afx_msg void OnBtnStopRoomClick();
	afx_msg void OnBtnStartAllClick();
	afx_msg void OnBtnStopAllClick();
	afx_msg void OnBtnUpdateListClick();
	DECLARE_MESSAGE_MAP()

protected:
	CListCtrl		m_gameList;
	CListCtrl		m_roomList;
	bool			m_bInited;
	char			m_szHostIP[20];
};

#endif // __GAMELIST_DIALOG_H__
