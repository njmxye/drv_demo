#include<ntifs.h>
#include<ntddk.h>

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

NTSTATUS DispatchWrite(PDEVICE_OBJECT DeviceObject, PIRP pIrp) {
	UNREFERENCED_PARAMETER(DeviceObject);
	 
	
	PVOID mdl_buff = MmGetMdlVirtualAddress(pIrp->MdlAddress);
	ULONG len = MmGetMdlByteCount(pIrp->MdlAddress); 
	if (!MmIsAddressValid(mdl_buff)) {
		DbgPrint("mdl_buff is null！\n");
		pIrp->IoStatus.Information = 0;
		pIrp->IoStatus.Status = STATUS_SUCCESS;
		IoCompleteRequest(pIrp, IO_NO_INCREMENT);
		return STATUS_SUCCESS;
	}
	DbgPrint("mdl_buff is %s！\n", mdl_buff);
	pIrp->IoStatus.Information = len;
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
	
	PDeviceObj->Flags |= DO_DIRECT_IO ; // 将 PDeviceObj->Flags 设置为 0，意味着清除所有的标志位，将设备对象恢复到默认状态。

	//创建 R0 与 R3 的符号链接 
	status = IoCreateSymbolicLink(&symbolLINKName, &DeviceName);
	if (!NT_SUCCESS(status)) {
		return status;
	}
	//例程
	DbgPrint("驱动加载成功！\n");
	DriverObject->MajorFunction[IRP_MJ_CREATE] = DispatchCreate;
	DbgPrint("驱动加载成功！\n");
	DriverObject->MajorFunction[IRP_MJ_CLOSE] = DispatchClose;
	DriverObject->MajorFunction[IRP_MJ_READ] = DispatchRead;
	DriverObject->MajorFunction[IRP_MJ_WRITE] = DispatchWrite;
	DbgPrint("驱动加载成功！\n");
	return STATUS_SUCCESS;
}