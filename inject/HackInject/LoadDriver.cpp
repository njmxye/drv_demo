#include "pch.h"
#include "LoadDriver.h"

BOOL LoadDriver(const char* lpszDrivername, const char* sysFileName)
{
    BOOL bREt = TRUE;
    SC_HANDLE hService = NULL; 
    SC_HANDLE hServiceDDK = NULL;
    char szDriverFilePAth[256] = { 0 };
    GetFullPathNameA(sysFileName, sizeof(szDriverFilePAth), szDriverFilePAth, NULL); 
    //打开服务控制管理器数据库（SCM）
    hService = OpenSCManagerA(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (hService == NULL) {
        OutputDebugStringA("Open SCM failed \n");
        bREt = FALSE; 
        goto  Clean;
    }


    
    hServiceDDK = CreateServiceA(hService,
        lpszDrivername,
        lpszDrivername,
        SC_MANAGER_ALL_ACCESS,
        SERVICE_KERNEL_DRIVER,
        SERVICE_DEMAND_START,
        SERVICE_ERROR_NORMAL,
        szDriverFilePAth,
        NULL, NULL, NULL, NULL, NULL);

    if (hServiceDDK == NULL) {
        if (GetLastError() == ERROR_SERVICE_EXISTS) {
            hServiceDDK = OpenServiceA(hService, lpszDrivername, SERVICE_ALL_ACCESS);

        }
        else {
            OutputDebugStringA("Start SCM failed \n");
            bREt = FALSE;
            goto  Clean;
        }
    }

    bREt = StartServiceA(hServiceDDK, NULL, NULL);
    if (!bREt) {
        if (GetLastError() != ERROR_IO_PENDING && GetLastError() != ERROR_SERVICE_ALREADY_RUNNING) {
            OutputDebugStringA("start service failed \n");
            bREt = FALSE; 
            goto  Clean;
        }
        else {
            return TRUE;
        }
    }
    bREt = TRUE;

Clean:
    if (hServiceDDK) CloseServiceHandle(hServiceDDK);
    if (hService) CloseServiceHandle(hService);
    

    return bREt;
}

BOOL UnloadDriver(const char* lpszDriverName)
{
    BOOL bREt = TRUE;
    SC_HANDLE hService = NULL;
    SERVICE_STATUS svstat;
    SC_HANDLE hServiceDDK = NULL;
    //打开服务控制管理器数据库（SCM）
    hService = OpenSCManagerA(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (hService == NULL) {
        OutputDebugStringA("Open SCM failed \n");
        bREt = FALSE;
        goto  Clean;
    }



    
    hServiceDDK = OpenServiceA(hService, lpszDriverName, SERVICE_ALL_ACCESS); // 停止一个服务器 需要打开

    if (hServiceDDK == NULL) {
        OutputDebugStringA("open SCM failed \n");
        bREt = FALSE;
        goto  Clean;
    }

    if (!ControlService(hServiceDDK, SERVICE_CONTROL_STOP, &svstat)) {
        OutputDebugStringA("stop SCM failed \n");
        bREt = FALSE;
        goto  Clean;
    }
    
    if (!DeleteService(hServiceDDK)) {
        OutputDebugStringA("Delete SCM failed \n");
        bREt = FALSE;
        goto  Clean;
    }

    bREt = TRUE;

Clean:
    if (hServiceDDK) CloseServiceHandle(hServiceDDK);
    if (hService) CloseServiceHandle(hService);


    return bREt;
}
