#include "comm.h"


#define MAX_PATH 260
namespace common {

	// Call   BOOLEAN result = PrepMiniFilter(my_driver_reg_path, L"324213");
	auto  PrepMiniFilter(IN PUNICODE_STRING reg_path, IN PCWSTR altiude)-> NTSTATUS {

		BOOLEAN result = FALSE;
		NTSTATUS status = STATUS_UNSUCCESSFUL;
		PWSTR driver_name = NULL;
		WCHAR key_path[MAX_PATH] = { 0 };
		WCHAR default_instance_value_data[MAX_PATH] = { 0 };
		if (reg_path == NULL) return result;
		if (reg_path->Buffer == NULL) return result;
		if (reg_path->Length <= 0) return result;
		if (altiude == NULL) return result;
		if (altiude[0] == L'\0') return result;

		do {
			driver_name = wcsrchr(reg_path->Buffer, L'\\');
			if (!MmIsAddressValid(driver_name)) break;

			RtlZeroMemory(key_path, MAX_PATH * sizeof(WCHAR));
			// swprintf(key_path, L"%ws\\Instances", driver_name);
			status = RtlStringCbPrintfW(key_path, sizeof(key_path), L"%ws\\Instances",
				driver_name);
			if (!NT_SUCCESS(status)) break;
			status = RtlCreateRegistryKey(RTL_REGISTRY_SERVICES, key_path);
			if (!NT_SUCCESS(status)) break;

			//  swprintf(Data, L"%ws Instance", &driver_name[1]);
			status = RtlStringCbPrintfW(default_instance_value_data,
				sizeof(default_instance_value_data),
				L"%ws Instance", &driver_name[1]);
			if (!NT_SUCCESS(status)) break;

			status = RtlWriteRegistryValue(
				RTL_REGISTRY_SERVICES, key_path, L"DefaultInstance", REG_SZ,
				default_instance_value_data,
				(ULONG)(wcslen(default_instance_value_data) * sizeof(WCHAR) + 2));

			if (!NT_SUCCESS(status)) break;
			RtlZeroMemory(key_path, MAX_PATH * sizeof(WCHAR));

			// swprintf(key_path, L"%ws\\Instances%ws Instance", driver_name,
			// driver_name);
			status = RtlStringCbPrintfW(key_path, sizeof(key_path),
				L"%ws\\Instances%ws Instance", driver_name,
				driver_name);
			if (!NT_SUCCESS(status)) break;

			status = RtlCreateRegistryKey(RTL_REGISTRY_SERVICES, key_path);
			if (!NT_SUCCESS(status)) break;

			status = RtlCreateRegistryKey(RTL_REGISTRY_SERVICES, key_path);
			if (!NT_SUCCESS(status)) break;

			status = RtlWriteRegistryValue(
				RTL_REGISTRY_SERVICES, key_path, L"Altitude", REG_SZ, &altiude,
				(ULONG)(wcslen(altiude) * sizeof(WCHAR) + 2));
			if (!NT_SUCCESS(status)) break;
			ULONG dwData = 0;
			status = RtlWriteRegistryValue(RTL_REGISTRY_SERVICES, key_path, L"Flags",
				REG_DWORD, &dwData, 4);
			if (!NT_SUCCESS(status)) break;
			result = TRUE;
		} while (FALSE);

		return result;
	}



	/*-----------------------------------------*/

	auto  connectCallBack(
		PFLT_PORT ClientPort,
		PVOID ServerPortCookie,
		PVOID ConnectionContext,
		ULONG SizeOfContext,
		PVOID* ConnectionPortCookie
	)->NTSTATUS{

		UNREFERENCED_PARAMETER(ServerPortCookie);
		UNREFERENCED_PARAMETER(ConnectionContext);
		UNREFERENCED_PARAMETER(ConnectionContext);
		UNREFERENCED_PARAMETER(SizeOfContext);
		UNREFERENCED_PARAMETER(ConnectionPortCookie);

		ClientPort = ClientPort;
		DbgPrint("connect clinet ");
		return STATUS_SUCCESS;
	}

	 auto msgCallBack(
		PVOID PortCookie,
		PVOID InputBuffer,
		ULONG InputBufferLength,
		PVOID OutputBuffer /*传递给给 r3 的数据*/ , 
		ULONG OutputBufferLength,
		PULONG ReturnOutputBufferLength )->NTSTATUS{
		
		 UNREFERENCED_PARAMETER(PortCookie);
		 UNREFERENCED_PARAMETER(InputBuffer);
		 UNREFERENCED_PARAMETER(InputBufferLength);
		 UNREFERENCED_PARAMETER(OutputBuffer);
		 UNREFERENCED_PARAMETER(OutputBufferLength);
		 UNREFERENCED_PARAMETER(ReturnOutputBufferLength);

		 // 拿到r3 的pid， 通过 pid 伪造句柄返回


		 DbgPrint("msgCallBack");    // 传递伪造的句柄给三环
		 return CommonFunc(InputBuffer, InputBufferLength, OutputBuffer, OutputBufferLength, ReturnOutputBufferLength); // 在DrvMain 里调用 createCommonPort 就会注册这些函数
	}

	auto disconnectCallBack(PVOID ConnectionCookie){
		UNREFERENCED_PARAMETER(ConnectionCookie);
		FltCloseClientPort(fltFilter,&ClientPort);
		DbgPrint("close  client port ");
	}

	auto miniUnload(FLT_FILTER_UNLOAD_FLAGS Flags)->NTSTATUS {
		DbgPrint("hi\n"); 

		FltCloseCommunicationPort(drvPort);
		FltUnregisterFilter(fltFilter);
		return STATUS_SUCCESS;
	}


	auto createCommonPort(PDRIVER_OBJECT driverObject, PUNICODE_STRING RegisterPath, fnCommoFunc func) -> NTSTATUS {

		UNREFERENCED_PARAMETER(RegisterPath);

		if (func == nullptr) {
			return STATUS_UNSUCCESSFUL;
		}
		CommonFunc = func;
		PrepMiniFilter(RegisterPath, L"324210");

		NTSTATUS status;
		auto freg = FLT_REGISTRATION{
			sizeof(FLT_REGISTRATION),
			FLT_REGISTRATION_VERSION,
			NULL,
			NULL,
			NULL,
			miniUnload,
			nullptr,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL

		};

		status = FltRegisterFilter(driverObject,&freg,&fltFilter);

		if (NT_SUCCESS(status)) {
			OBJECT_ATTRIBUTES oa{ 0 };
			UNICODE_STRING portName = RTL_CONSTANT_STRING(L"\\mf");
			PSECURITY_DESCRIPTOR sd = { nullptr };
			status = FltBuildDefaultSecurityDescriptor(&sd, FLT_PORT_ALL_ACCESS);
				if (!NT_SUCCESS(status)) {
					DbgPrint("fail to create sd \n");
					FltUnregisterFilter(fltFilter);
					FltFreeSecurityDescriptor(sd);
					return status;
				}

				InitializeObjectAttributes(&oa, &portName, OBJ_KERNEL_HANDLE | OBJ_CASE_INSENSITIVE, nullptr, sd);		//对象属性初始化

				status = FltCreateCommunicationPort(fltFilter, &drvPort, &oa, nullptr, connectCallBack, disconnectCallBack, msgCallBack, 1);
				if (!NT_SUCCESS(status)) {
					DbgPrint("fail to Create Communication Port \n");
					FltUnregisterFilter(fltFilter);
					FltFreeSecurityDescriptor(sd);
					return status;
				}
				FltFreeSecurityDescriptor(sd);
		}
		return status;
	}
}