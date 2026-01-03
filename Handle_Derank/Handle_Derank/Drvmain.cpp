#include <ntifs.h>
#include <ntddk.h>


PVOID g_callBack_Handle = 0;
HANDLE g_pid = (HANDLE)496;

NTSTATUS DispatchCreate(PDRIVER_OBJECT DriverObject, PIRP pIrp) {
	pIrp->IoStatus.Information = 0; 
	pIrp->IoStatus.Status = STATUS_SUCCESS;

	DbgPrint("驱动加载成功");

	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;

}

void Unload(PDRIVER_OBJECT DriverObjec) {
	UNREFERENCED_PARAMETER(DriverObjec);
	if (g_callBack_Handle != 0) {
		ObUnRegisterCallbacks(g_callBack_Handle);
	}

}

OB_PREOP_CALLBACK_STATUS ObProceeCallBack(PVOID RegistrationContext, POB_PRE_OPERATION_INFORMATION OperationInformation) {
	UNREFERENCED_PARAMETER(RegistrationContext);
	PEPROCESS process{ 0 };
	PsLookupProcessByProcessId(g_pid, &process);
	if (OperationInformation->Operation == OB_OPERATION_HANDLE_CREATE) {
		if (OperationInformation->Object == process) {

			OperationInformation->Parameters->CreateHandleInformation.DesiredAccess = 0;
		}
	}
	return OB_PREOP_SUCCESS;

}

EXTERN_C NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegisterPath) {
	ULONG_PTR pDrvSection = (ULONG_PTR)DriverObject->DriverSection;
	*(PULONG)(pDrvSection + 0x68) |= 0x20; /* From https ://blog.csdn.net/qq_41252520/article/details/134386049 */

	UNREFERENCED_PARAMETER(RegisterPath);

	PDEVICE_OBJECT PDeviceObj;
	UNICODE_STRING DeviceName = RTL_CONSTANT_STRING(L"\\Device\\Handle_Derank");
	UNICODE_STRING SymbolLink = RTL_CONSTANT_STRING(L"\\??\\Handle_Derank");

	NTSTATUS status = IoCreateDevice(DriverObject, NULL, &DeviceName, FILE_DEVICE_UNKNOWN, NULL, FALSE, &PDeviceObj);
	if (!NT_SUCCESS(status)) {
		return status; 
	}

	status = IoCreateSymbolicLink(&SymbolLink, &DeviceName);
	if (!NT_SUCCESS(status)) {
		return status;
	}

	OB_CALLBACK_REGISTRATION callBackReg{ 0 };
	OB_OPERATION_REGISTRATION callbackOper{ 0 };


	callBackReg.Version = OB_FLT_REGISTRATION_VERSION;
	callBackReg.OperationRegistrationCount = 1;
	callBackReg.Altitude = RTL_CONSTANT_STRING(L"371055.1351");
	callBackReg.RegistrationContext = nullptr;
	callBackReg.OperationRegistration = &callbackOper;

	callbackOper.ObjectType = PsProcessType;
	callbackOper.Operations = OB_OPERATION_HANDLE_CREATE | OB_OPERATION_HANDLE_DUPLICATE;
	callbackOper.PreOperation = ObProceeCallBack;
	callbackOper.PostOperation = nullptr;

	/*UNICODE_STRING DestinationString;
	RtlInitUnicodeString(&DestinationString, L"371055.1351")*/


	DriverObject->DriverUnload = Unload;




	return ObRegisterCallbacks(&callBackReg, &g_callBack_Handle);
}