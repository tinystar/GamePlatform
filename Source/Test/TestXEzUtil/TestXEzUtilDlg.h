
// TestXEzUtilDlg.h : 头文件
//

#pragma once


// CTestXEzUtilDlg 对话框
class CTestXEzUtilDlg : public CDialogEx
{
// 构造
public:
	CTestXEzUtilDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_TESTXEZUTIL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();

	afx_msg void OnBnClickedBtnMem1();
	afx_msg void OnBnClickedBtnMem2();
	DECLARE_MESSAGE_MAP()
};
