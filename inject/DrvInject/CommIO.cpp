#include"CommIO.h"



NTSTATUS DispatchCreate(PDEVICE_OBJECT DeviceObject, PIRP pIrp) {
	UNREFERENCED_PARAMETER(DeviceObject);
	//返回给3ring
	pIrp->IoStatus.Information = 0;
	pIrp->IoStatus.Status = STATUS_SUCCESS;

	DbgPrint("DispatchCreate！\n");

	IoCompleteRequest(pIrp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;

}

NTSTATUS DispatchClose(PDEVICE_OBJECT DeviceObject, PIRP pIrp) {
	UNREFERENCED_PARAMETER(DeviceObject);
	//返回给3ring
	pIrp->IoStatus.Information = 0;
	pIrp->IoStatus.Status = STATUS_SUCCESS;

	DbgPrint("DispatchClose！\n");

	IoCompleteRequest(pIrp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;

}

NTSTATUS DispatchControl(PDEVICE_OBJECT DeviceObject, PIRP pIrp) {
	UNREFERENCED_PARAMETER(DeviceObject);
	ULONG length = 0;
	NTSTATUS status = STATUS_SUCCESS;
	auto stack = IoGetCurrentIrpStackLocation(pIrp);

	switch (stack->Parameters.DeviceIoControl.IoControlCode) 
	{
	case CALLBACKINJECT: {
		
		PINIT_DATA info = (PINIT_DATA)pIrp->AssociatedIrp.SystemBuffer;
		g_fnLoadLibrary = info->fnLoadLibrary; 
		g_fnGetProcAddress = info->fnGetProcAddress;
		g_fnRtlAddFunction = info->fnRtlAddFunction;

		if (g_fnLoadLibrary == 0 || g_fnGetProcAddress == 0 || g_fnRtlAddFunction == 0) {
			DbgPrint("g_fnGetProcAddress %p,g_fnLoadLibrary:%p,g_fnAddFuntionTable:%p", g_fnGetProcAddress, g_fnLoadLibrary, g_fnRtlAddFunction);
			return STATUS_UNSUCCESSFUL;
		}
	
		wchar_t DllR0Name[MAX_PATH] = { 0 }; // whar_t unicode 宽字符 
		wcscpy(DllR0Name, L"\\??\\");
		wcscat(DllR0Name, info->szDllName);
		UNICODE_STRING r0_dll_path{ 0 };
		RtlInitUnicodeString(&r0_dll_path, DllR0Name);

		status = inst_callback_inject((HANDLE)info->dwPid, &r0_dll_path);

		length = sizeof(PINIT_DATA);
		break; 
	}
	default:
		return status;
	}

		//DbgBreakPoint();
	
	pIrp->IoStatus.Information = length;
	pIrp->IoStatus.Status = status;



	IoCompleteRequest(pIrp, IO_NO_INCREMENT);

	return status;

}