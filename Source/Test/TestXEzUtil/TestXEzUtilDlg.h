
// TestXEzUtilDlg.h : ͷ�ļ�
//

#pragma once


// CTestXEzUtilDlg �Ի���
class CTestXEzUtilDlg : public CDialogEx
{
// ����
public:
	CTestXEzUtilDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_TESTXEZUTIL_DIALOG };

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

	afx_msg void OnBnClickedBtnMem1();
	afx_msg void OnBnClickedBtnMem2();
	DECLARE_MESSAGE_MAP()
};
