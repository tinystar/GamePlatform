
// TimerTestDlg.h : ͷ�ļ�
//

#pragma once


// CTimerTestDlg �Ի���
class CTimerTestDlg : public CDialogEx
{
// ����
public:
	CTimerTestDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_TIMERTEST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	afx_msg LRESULT OnUpdateInterval(WPARAM wParam, LPARAM lParam);

protected:
	UINT		m_uDuration;
	double		m_dInterval;
};
