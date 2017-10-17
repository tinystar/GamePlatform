
// ChildView.h : CChildView 类的接口
//


#pragma once
#include "MainServerMgr.h"


enum ServerStatus
{
	kInited		= 0x01,
	kStarted	= 0x02
};

#define WM_FIRST_STARTSVR		(WM_USER + 1)

// CChildView 窗口

class CChildView : public CWnd, public IMainServerUIObserver
{
// 构造
public:
	CChildView();

// 特性
public:

// 操作
public:

// 重写
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// 实现
public:
	virtual ~CChildView();

protected:
	virtual void onUIConnToGateSuccess();
	virtual void onUIConnToGateFail(int nErrCode);
	virtual void onUIConnToGateClosed();

	virtual void onUIConnToDBSuccess();
	virtual void onUIConnToDBFail(int nErrCode);
	virtual void onUIConnToDBClosed();

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
	afx_msg LRESULT OnConnToGateSuccessUIMsg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnConnToGateFailUIMsg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnConnToGateClosedUIMsg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnConnToDBSuccessUIMsg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnConnToDBFailUIMsg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnConnToDBClosedUIMsg(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

protected:
	MainServerMgr			m_mainSvrMgr;
	int						m_svrStatus;
	CButton					m_startBtn;
	CButton					m_dumpBtn;
	CButton					m_openDirBtn;
	CButton					m_updCfgBtn;
	CStatic					m_portLabel;
	CEdit					m_portEdit;
	CStatic					m_gateAddrLabel;
	CStatic					m_gatePortLabel;
	CEdit					m_gateAddrEdit;
	CEdit					m_gatePortEdit;
	CStatic					m_DBAddrLabel;
	CStatic					m_DBPortLabel;
	CEdit					m_DBAddrEdit;
	CEdit					m_DBPortEdit;
	CStatic					m_maxUserLabel;
	CEdit					m_maxUserEdit;
	CEdit					m_svrMsgEdit;
	CFont					m_font;
	BOOL					m_bFirstStart;
};

