#include<ntifs.h>
#include<ntddk.h>
#define wtire CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED,FILE_ANY_ACCESS )
#define apc CTL_CODE(FILE_DEVICE_UNKNOWN, 0x802, METHOD_BUFFERED,FILE_ANY_ACCESS )
void DriverUnload(PDRIVER_OBJECT DriverObject) {

	if (DriverObject->DeviceObject) {
		IoDeleteDevice(DriverObject->DeviceObject);
		UNICODE_STRING symbolLinkName = RTL_CONSTANT_STRING(L"\\??\\DemoInject");
		NTSTATUS status = IoDeleteSymbolicLink(&symbolLinkName);
		if (!NT_SUCCESS(status)) {
			DbgPrint("符号删除成功！\n");
		}
	}
	DbgPrint("驱动卸载成功！\n");
}

NTSTATUS DispatchClose(PDEVICE_OBJECT DeviceObject, PIRP pIrp) {
	UNREFERENCED_PARAMETER(DeviceObject);
	//返回给3ring
	pIrp->IoStatus.Information = 0;
	pIrp->IoStatus.Status = STATUS_SUCCESS;

	DbgPrint("DispatchCreate！\n");
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;

}

NTSTATUS DispatchCreate(PDEVICE_OBJECT DeviceObject , PIRP pIrp) {
	UNREFERENCED_PARAMETER(DeviceObject);
	//返回给3ring
	pIrp->IoStatus.Information = 0;
	pIrp->IoStatus.Status = STATUS_SUCCESS;

	DbgPrint("DispatchCreate！\n");
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;

}

NTSTATUS DispatchRead(PDEVICE_OBJECT DeviceObject, PIRP pIrp) {
	UNREFERENCED_PARAMETER(DeviceObject);

	char buff[255] = "hello world form r0 \n";

	PVOID sysBuff = pIrp->AssociatedIrp.SystemBuffer;
	if (!MmIsAddressValid(sysBuff)) {
		DbgPrint("sysBuff is null！\n");
		pIrp->IoStatus.Information = 0;
		pIrp->IoStatus.Status = STATUS_SUCCESS;
		IoCompleteRequest(pIrp, IO_NO_INCREMENT);
		return STATUS_SUCCESS;
	}
	RtlCopyMemory(sysBuff, buff, sizeof(buff));
	pIrp->IoStatus.Information = sizeof(buff);
	pIrp->IoStatus.Status = STATUS_SUCCESS;
	return STATUS_SUCCESS;

 }

NTSTATUS DispatchCTL(PDEVICE_OBJECT DeviceObject, PIRP pIrp) {
	UNREFERENCED_PARAMETER(DeviceObject);

	char buff[255] = "hello world form r0";
	PVOID sysBuff = pIrp->AssociatedIrp.SystemBuffer;
	auto stack = IoGetCurrentIrpStackLocation(pIrp);
	int length = stack->Parameters.Read.Length;
	switch (stack->Parameters.DeviceIoControl.IoControlCode) {
		case wtire: {
			if (!MmIsAddressValid(sysBuff)) {
				DbgPrint("sysBuff is null！\n");
				pIrp->IoStatus.Information = 0;
				pIrp->IoStatus.Status = STATUS_SUCCESS;
				IoCompleteRequest(pIrp, IO_NO_INCREMENT);
				return STATUS_SUCCESS;
			}
			DbgPrint("sysBuff is %s！\n", sysBuff);
			RtlCopyMemory(sysBuff, buff, sizeof(buff));
			break;
		}
		default:
			break;
	}
	pIrp->IoStatus.Information = length;
	pIrp->IoStatus.Status = STATUS_SUCCESS;
	IoCompleteRequest(pIrp, IO_NO_INCREMENT); // !!! 
	return STATUS_SUCCESS;
	

}

NTSTATUS DispatchWrite(PDEVICE_OBJECT DeviceObject, PIRP pIrp) {
	/*UNREFERENCED_PARAMETER(DeviceObject);
	 
	auto stack = IoGetCurrentIrpStackLocation(pIrp);
	stack->Parameters.Read.Length;

	PVOID sysBuff = pIrp->AssociatedIrp.SystemBuffer;
	 if (!MmIsAddressValid(sysBuff) && stack > 0) {
		DbgPrint("sysBuff is null！\n");
		pIrp->IoStatus.Information = 0;
		pIrp->IoStatus.Status = STATUS_SUCCESS;
		IoCompleteRequest(pIrp, IO_NO_INCREMENT);
		return STATUS_SUCCESS;
	}
	DbgPrint("sysBuff is %p！\n", sysBuff);
	pIrp->IoStatus.Information = sizeof(stack);
	pIrp->IoStatus.Status = STATUS_SUCCESS;
	return STATUS_SUCCESS;*/
	UNREFERENCED_PARAMETER(DeviceObject);
	auto buff = pIrp->UserBuffer;
	auto stack = IoGetCurrentIrpStackLocation(pIrp);
	stack->Parameters.Read.Length;
	
	if (!MmIsAddressValid(buff) && stack > 0) {
		DbgPrint("sysBuff is null！\n");
		pIrp->IoStatus.Information = 0;
		pIrp->IoStatus.Status = STATUS_SUCCESS;
		IoCompleteRequest(pIrp, IO_NO_INCREMENT);
		return STATUS_SUCCESS;
	}
	DbgPrint("direct Buff is %p！\n", pIrp->UserBuffer);
	DbgPrint("direct Buff is %p！\n", buff);
	pIrp->IoStatus.Information = sizeof(stack);
	pIrp->IoStatus.Status = STATUS_SUCCESS;
	return STATUS_SUCCESS; 
}




EXTERN_C NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegisterPath) {
	RegisterPath;
	DriverObject->DriverUnload = DriverUnload;

	NTSTATUS status;
	PDEVICE_OBJECT PDeviceObj;
	UNICODE_STRING DeviceName = RTL_CONSTANT_STRING(L"\\Device\\DemoInject");
	UNICODE_STRING symbolLINKName = RTL_CONSTANT_STRING(L"\\??\\DemoInject");

	//创建设备对象
	status = IoCreateDevice(DriverObject, NULL, &DeviceName, FILE_DEVICE_UNKNOWN, NULL, FALSE, &PDeviceObj);
	if (!NT_SUCCESS(status)){
		return status ;
	}
	
	//PDeviceObj->Flags |= DO_BUFFERED_IO ; // 将 PDeviceObj->Flags 设置为 0，意味着清除所有的标志位，将设备对象恢复到默认状态。
	//PDeviceObj->Flags = 0; not unnecessary 
	//创建 R0 与 R3 的符号链接 
	status = IoCreateSymbolicLink(&symbolLINKName, &DeviceName);
	if (!NT_SUCCESS(status)) {
		return status;
	}
	//例程
	DriverObject->MajorFunction[IRP_MJ_CREATE] = DispatchCreate;
	DriverObject->MajorFunction[IRP_MJ_CLOSE] = DispatchClose;
	//DriverObject->MajorFunction[IRP_MJ_READ] = DispatchRead;
	//DriverObject->MajorFunction[IRP_MJ_WRITE] = DispatchWrite;
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DispatchCTL;
	DbgPrint("驱动加载成功！\n");
	return STATUS_SUCCESS;
}