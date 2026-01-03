
// HackInjectDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "HackInject.h"
#include "HackInjectDlg.h"
#include "afxdialogex.h"
#include"LoadDriver.h"
#include "CommIO.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#define WM_PROCESS (WM_USER+1)
#define DRIVERNAME "DrvInject"
// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CHackInjectDlg 对话框



CHackInjectDlg::CHackInjectDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_HACKINJECT_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CHackInjectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_EDIT1, m_DllPath);

	DDX_Control(pDX, IDC_EDIT2, m_ProcessID);
}

BEGIN_MESSAGE_MAP(CHackInjectDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
//	ON_BN_CLICKED(IDC_BUTTON1, &CHackInjectDlg::OnBnClickedButton1)
ON_BN_CLICKED(BN_MODULE, &CHackInjectDlg::OnBnClickedModule)
ON_BN_CLICKED(BN_INJECT, &CHackInjectDlg::OnBnClickedInject)
ON_BN_CLICKED(BN_PID, &CHackInjectDlg::OnBnClickedPid)
ON_MESSAGE(WM_PROCESS, &CHackInjectDlg::OnProcess)
ON_WM_CLOSE()
END_MESSAGE_MAP()


// CHackInjectDlg 消息处理程序

BOOL CHackInjectDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	SetWindowText(L"驱动注入器 - mnnna");

	if (!LoadDriver(DRIVERNAME, "DrvInject.sys")) {
		GetDlgItem(IDC_STATIC_TIP)->SetWindowTextW(L"驱动加载失败");
		return FALSE;
	}

	if (!InitDriver()) {
		GetDlgItem(IDC_STATIC_TIP)->SetWindowTextW(L"驱动启动失败");
		return FALSE;
	}


	GetDlgItem(IDC_STATIC_TIP)->SetWindowTextW(L"驱动加载成功");

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CHackInjectDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CHackInjectDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CHackInjectDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



//void CHackInjectDlg::OnBnClickedButton1()
//{
//	// TODO: 在此添加控件通知处理程序代码
//}


void CHackInjectDlg::OnBnClickedModule()
{
	CFileDialog fildig(true, L"dll", L"*.dll", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"dll Files(*.dll)", nullptr);
	if (fildig.DoModal() == IDOK)//谈对话框
	{
		m_DllPath.SetWindowText(fildig.GetPathName());

	}

	// TODO: 在此添加控件通知处理程序代码
}


void CHackInjectDlg::OnBnClickedInject()
{
	// TODO: 在此添加控件通知处理程序代码
	CString ModuleName;

	m_DllPath.GetWindowTextW(ModuleName);

	if (ModuleName.IsEmpty())
	{
		GetDlgItem(IDC_STATIC_TIP)->SetWindowTextW(L"DLL路径为空，请选择DLL!");
		return;
	}
	CString StrPid;

	m_ProcessID.GetWindowTextW(StrPid);

	if (StrPid.IsEmpty())
	{
		GetDlgItem(IDC_STATIC_TIP)->SetWindowTextW(L"进程ID为空 ,请输入进程ID!");
		return;
	}
	DWORD PID = _wtoi(StrPid);//进程ID   字符转 DWORD64 PID

	wchar_t* DllPath = ModuleName.GetBuffer(ModuleName.GetLength());

	



	if (((CButton*)GetDlgItem(IDC_RADIO1))->GetCheck())
	{
		if (CallBackInject(PID ,DllPath)) {
			GetDlgItem(IDC_STATIC_TIP)->SetWindowTextW(L"注入成功! ");
		}
	}

}


void CHackInjectDlg::OnBnClickedPid()
{
	// TODO: 在此添加控件通知处理程序代码
	CProcessList processList;
	processList.DoModal();
}


afx_msg LRESULT CHackInjectDlg::OnProcess(WPARAM wParam, LPARAM lParam)
{
	
	//UpdateData(TRUE);
	////m_PID=wParam;
	//UpdateData(FALSE);
	
	//WPARAM wParamValue = ...; // 你的 WPARAM 值
	TCHAR buffer[256];
	wsprintf(buffer, _T("%lu"), wParam);
	m_ProcessID.SetWindowTextW(buffer);
	return 0;
}



void CHackInjectDlg::OnClose()
{

	if (MessageBox(L"确定要退出程序并卸载驱动吗？", L"退出提示", MB_ICONINFORMATION | MB_YESNO) == IDNO)
	{
		return; //注意无返回值
	}
	StopDriver();
	if (!UnloadDriver(DRIVERNAME)) {
		MessageBox(L"卸载失败", L"退出", MB_ICONEXCLAMATION | MB_YESNO);
	}
	CDialogEx::OnClose();
}
