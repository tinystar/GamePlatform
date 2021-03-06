
// ChildView.h : CChildView 类的接口
//


#pragma once
#include "DBServerMgr.h"


enum ServerStatus
{
	kInited		= 0x01,
	kStarted	= 0x02
};

#define WM_FIRST_STARTSVR		(WM_USER + 1)

// CChildView 窗口

class CChildView : public CWnd, public IDBServerUIObserver
{
// 构造
public:
	CChildView();

// 特性
public:

// 操作
public:
	DBServerMgr& getServerMgr() { return m_dbSvrMgr; }

// 重写
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// 实现
public:
	virtual ~CChildView();

protected:
	virtual void onUIDatabaseOpen(bool bSucc, const TCHAR* pszMsg);
	virtual void onUIDatabaseClose();

protected:
	void AppendServerMsg(const TCHAR* pszMsg);

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
	afx_msg LRESULT OnDatabaseOpenUIMsg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDatabaseCloseUIMsg(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

protected:
	DBServerMgr				m_dbSvrMgr;
	int						m_svrStatus;
	CButton					m_startBtn;
	CButton					m_dumpBtn;
	CButton					m_openDirBtn;
	CButton					m_updCfgBtn;
	CEdit					m_msgEdit;
	CFont					m_font;
	BOOL					m_bFirstStart;
};

