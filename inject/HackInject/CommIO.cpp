#include"pch.h"
#include "CommIO.h"


HANDLE hDevice = NULL;

BOOL InitDriver()
{
		
	hDevice = CreateFileA(DEVICE_LINK_NAME, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, NULL, NULL);
	return hDevice ? TRUE : FALSE;

}

BOOL StopDriver()
{
	return CloseHandle(hDevice);
}

BOOL CallBackInject(DWORD PID, wchar_t* szDllName)
{
	INIT_DATA info = { 0 };
	info.dwPid = PID;
	info.szDllName = szDllName;

	HMODULE hk32 = LoadLibraryA("kernel32.dll");
	HMODULE hntdll = LoadLibraryA("ntdll.dll");

	info.fnGetProcAddress = (UINT64)GetProcAddress(hk32, "GetProcAddress");
	info.fnLoadLibrary = (UINT64)GetProcAddress(hk32, "LoadLibraryA");

	info.fnRtlAddFunction = (UINT64)GetProcAddress(hntdll, "RtlAddFunctionTable");
	// ?
	FreeLibrary(hk32);
	FreeLibrary(hntdll);

	BOOL bRet = DeviceIoControl(hDevice, CALLBACKINJECT, &info, sizeof(INIT_DATA), NULL, NULL, NULL, NULL);
	return bRet;
}
