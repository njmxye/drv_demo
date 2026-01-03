
// HackInjectDlg.h: 头文件
//

#pragma once


#include "CProcessList.h"




// CHackInjectDlg 对话框
class CHackInjectDlg : public CDialogEx
{
// 构造
public:
	CHackInjectDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_HACKINJECT_DIALOG };
#endif

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
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedModule();

	
	afx_msg void OnBnClickedInject();
	afx_msg void OnBnClickedPid();
protected:
	afx_msg LRESULT OnProcess(WPARAM wParam, LPARAM lParam);
public:
	CEdit m_DllPath;
	CEdit m_ProcessID;

	afx_msg void OnClose();
};
