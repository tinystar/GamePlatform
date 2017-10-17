
// ChildView.h : CChildView ��Ľӿ�
//


#pragma once
#include "DBServerMgr.h"


enum ServerStatus
{
	kInited		= 0x01,
	kStarted	= 0x02
};

#define WM_FIRST_STARTSVR		(WM_USER + 1)

// CChildView ����

class CChildView : public CWnd
{
// ����
public:
	CChildView();

// ����
public:

// ����
public:
	DBServerMgr& getServerMgr() { return m_dbSvrMgr; }

// ��д
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// ʵ��
public:
	virtual ~CChildView();

	// ���ɵ���Ϣӳ�亯��
protected:
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnBtnStartClick();
	afx_msg void OnBtnDumpClick();
	afx_msg void OnBtnUpdCfgClick();
	afx_msg void OnBtnOpenDirClick();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
	DECLARE_MESSAGE_MAP()

protected:
	DBServerMgr				m_dbSvrMgr;
	int						m_svrStatus;
	CButton					m_startBtn;
	CButton					m_dumpBtn;
	CButton					m_openDirBtn;
	CButton					m_updCfgBtn;
	CFont					m_font;
	BOOL					m_bFirstStart;
};

