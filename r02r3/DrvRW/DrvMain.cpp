#include <ntifs.h>
#include <ntddk.h>
#include "comm.h"
#include "rw.h"


EXTERN_C NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegisterPath) {
	RegisterPath;



	return common::createCommonPort(DriverObject, RegisterPath, rw::commonIO);
}