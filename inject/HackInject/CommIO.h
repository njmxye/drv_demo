#pragma once
#include <Windows.h>
#include <winioctl.h>

BOOL InitDriver();
BOOL StopDriver();

#define  DEVICE_LINK_NAME "\\\\.\\DemoInject"
#define CALLBACKINJECT CTL_CODE(FILE_DEVICE_UNKNOWN,0x801,METHOD_BUFFERED,FILE_ANY_ACCESS)

typedef struct _INT_DATA {
	DWORD32 dwPid;
	wchar_t* szDllName;

	UINT64 fnLoadLibrary;
	UINT64 fnGetProcAddress;
	UINT64 fnRtlAddFunction;
}INIT_DATA,* PINIT_DATA;

BOOL CallBackInject(DWORD PID, wchar_t* szDllName);