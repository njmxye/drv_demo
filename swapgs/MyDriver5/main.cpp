#include<ntifs.h>
#include<ntddk.h>

EXTERN_C void my_swapgs(UINT64* gs_base, UINT64* ke_gs_base);
void  DriverUnload(PDRIVER_OBJECT DriverObject) {
	DriverObject;
}

EXTERN_C NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING Register) {
	Register;

	DriverObject->DriverUnload = DriverUnload;
	UINT64 old_kernel_gs_base = 0, old_gs_base = 0, new_kernel_gs_base = 0, new_gs_base = 0; 

	my_swapgs(&old_kernel_gs_base, &old_gs_base);
	my_swapgs(&new_kernel_gs_base, &new_gs_base);

	DbgPrint("old: %p, old kernal:%p\n", old_gs_base, old_kernel_gs_base);
	DbgPrint("old: %p, old kernal:%p\n", new_gs_base, new_kernel_gs_base);
	return STATUS_SUCCESS; 
}