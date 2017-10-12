
// ChildView.h : CChildView ��Ľӿ�
//


#pragma once
#include "MainServerMgr.h"


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
	CFont					m_font;
	BOOL					m_bFirstStart;
};
