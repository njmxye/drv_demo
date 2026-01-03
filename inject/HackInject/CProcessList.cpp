// CProcessList.cpp: 实现文件
//

#include "pch.h"
#include "HackInject.h"
#include "CProcessList.h"
#include "afxdialogex.h"
#include <tlhelp32.h>

// CProcessList 对话框
#define WM_PROCESS (WM_USER+1)
IMPLEMENT_DYNAMIC(CProcessList, CDialogEx)

CProcessList::CProcessList(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG1, pParent)
{

}

CProcessList::~CProcessList()
{
}

void CProcessList::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST3, m_ProcessList);
}


BEGIN_MESSAGE_MAP(CProcessList, CDialogEx)
	ON_BN_CLICKED(IDC_OK, &CProcessList::OnBnClickedOk)
	ON_BN_CLICKED(IDC_FLUSH, &CProcessList::OnBnClickedFlush)
END_MESSAGE_MAP()


// CProcessList 消息处理程序


BOOL CProcessList::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_ProcessList.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_ProcessList.InsertColumn(0, L"进程名", 0, 170);
	m_ProcessList.InsertColumn(1, L"进程ID", 0, 80);

	InitProcessList();
	SetWindowText(L"进程列表");
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


VOID CProcessList::InitProcessList()
{
	// TODO: 在此处添加实现代码.
	m_ProcessList.DeleteAllItems();

	HANDLE hSapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	PROCESSENTRY32 PE = { sizeof(PROCESSENTRY32) };
	WCHAR wcth32ProcessID[256];
	ULONG UIndex = 0;
	BOOL boscess = Process32First(hSapshot, &PE);
	if (boscess == TRUE)
	{
		do
		{
			//wsprintf(wcth32ProcessID, L"%d", PE.th32ProcessID);
			swprintf_s(wcth32ProcessID, L"%d", PE.th32ProcessID);
			m_ProcessList.InsertItem(UIndex, PE.szExeFile);
			m_ProcessList.SetItemText(UIndex, 1, wcth32ProcessID);
			UIndex++;
		} while (Process32Next(hSapshot, &PE));

	}
	CloseHandle(hSapshot);
	return VOID();
	
}


void CProcessList::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	DWORD dwpot = (DWORD)m_ProcessList.GetFirstSelectedItemPosition();
	dwpot = dwpot - 1;
	CString ProcessID = m_ProcessList.GetItemText(dwpot, 1);

	int nprcessID = _ttoi(ProcessID);
	//Dialog
	CWnd* pWnd = FindWindow(nullptr, L"驱动注入器 - mnnna");

	HWND hwnd = pWnd->GetSafeHwnd();
	//向主界面传递消息
	::PostMessage(hwnd, WM_PROCESS, nprcessID, LPARAM(0));

	OnOK();//退出
}


void CProcessList::OnBnClickedFlush()
{
	// TODO: 在此添加控件通知处理程序代码
	InitProcessList();

	//CDialog::OnOK();
}

