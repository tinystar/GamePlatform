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

class GameListDialog : public CDialog
{
public:
	GameListDialog();

	enum { IDD = IDD_GAME_LIST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

	virtual BOOL OnInitDialog();

protected:
	afx_msg void OnBtnLoadModuleClick();
	afx_msg void OnBtnUnloadModuleClick();
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
};

#endif // __GAMELIST_DIALOG_H__
