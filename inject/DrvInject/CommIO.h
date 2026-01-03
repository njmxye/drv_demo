#pragma once
#include "ShellCode.h"
#include "InstrCallBack.h"

#define  DEVICENAME L"\\Device\\DemoInject"
#define  SYBOLNAME L"\\??\\DemoInject"

#define CALLBACKINJECT CTL_CODE(FILE_DEVICE_UNKNOWN,0x801,METHOD_BUFFERED,FILE_ANY_ACCESS)

EXTERN_C NTSTATUS DispatchCreate(PDEVICE_OBJECT DeviceObject, PIRP pIrp);
EXTERN_C NTSTATUS DispatchClose(PDEVICE_OBJECT DeviceObject, PIRP pIrp);
EXTERN_C NTSTATUS DispatchControl(PDEVICE_OBJECT DeviceObject, PIRP pIrp);

typedef struct _INT_DATA {
	DWORD32 dwPid; 
	wchar_t* szDllName;

	UINT64 fnLoadLibrary;
	UINT64 fnGetProcAddress;
	UINT64 fnRtlAddFunction;
}INIT_DATA, * PINIT_DATA;
