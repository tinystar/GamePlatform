
// ChildView.h : CChildView 类的接口
//


#pragma once
#include "GateServerMgr.h"
#include "IGateServerUIObserver.h"


enum ServerStatus
{
	kInited		= 0x01,
	kStarted	= 0x02
};

#define WM_FIRST_STARTSVR		(WM_USER + 1)

// CChildView 窗口

class CChildView : public CWnd, public IGateServerUIObserver
{
// 构造
public:
	CChildView();

// 特性
public:

// 操作
public:
	GateServerMgr& getServerMgr() { return m_gateSvrMgr; }

// 重写
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// 实现
public:
	virtual ~CChildView();

protected:
	virtual void onUIMainServerAdded(ClientId id, const MainSvrNode& svrNode);
	virtual void onUIMainServerRemoved(ClientId id);
	virtual void onUIMainUserLogin(ClientId id);
	virtual void onUIMainUserLogout(ClientId id);

protected:
	void updateOnlineUserDisplay(ClientId svrId, bool bLogin);

	// 生成的消息映射函数
protected:
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnBtnStartClick();
	afx_msg void OnBtnDumpClick();
	afx_msg void OnBtnUpdCfgClick();
	afx_msg void OnBtnOpenDirClick();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
	afx_msg LRESULT OnAddServerUIMsg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnRemoveServerUIMsg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMainUserLoginUIMsg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMainUserLogoutUIMsg(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

protected:
	GateServerMgr			m_gateSvrMgr;
	int						m_svrStatus;
	CButton					m_startBtn;
	CButton					m_dumpBtn;
	CButton					m_openDirBtn;
	CButton					m_updCfgBtn;
	CStatic					m_msvrLabel;
	CListCtrl				m_msvrList;
	CFont					m_font;
	BOOL					m_bFirstStart;
};

