#include "rw.h"

namespace rw {
	static auto VaToPa(void* va) -> uintptr_t{
		return MmGetPhysicalAddress(va).QuadPart;
	}

	static auto PaToVa(uintptr_t pa) -> void*{
		return MmGetVirtualForPhysical({ .QuadPart=(signed long long )pa});

	}
	static auto GetObHeaderCookie() -> ULONG {
		auto process = IoGetCurrentProcess();
		auto code1 = *(unsigned char*)((UINT_PTR)process - 0x18);
		auto code2 = (unsigned char)(((UINT_PTR)process - 0x30)>>8);

		return (7 ^ code1 ^ code2);
	}

	static auto GetVadOffset()-> ULONG {
		auto offset = 0ul; 
		UNICODE_STRING funcname = RTL_CONSTANT_STRING(L"PsGetProcessExitStatus");
		auto p = (PUCHAR)MmGetSystemRoutineAddress(&funcname);

		offset = *((PULONG)&p[2]);
		offset += 4;

		return offset;

	}

	static auto GetPebOffset() -> ULONG {
		auto offset = 0ul;
		UNICODE_STRING funcname = RTL_CONSTANT_STRING(L"PsGetProcessPeb");
		auto p = (PUCHAR)MmGetSystemRoutineAddress(&funcname);

		offset = *((PULONG)&p[3]);
		return offset;

	}

	auto commonIO(void* inbuf, ULONG inlen, void* outbuf, ULONG outlen, PULONG writenlen) -> NTSTATUS {
		NTSTATUS status;
		if (MmIsAddressValid(inbuf) || MmIsAddressValid(outbuf) || outlen < sizeof(HANDLE)) {
			status = STATUS_UNSUCCESSFUL;
			DbgPrint("arg is error \n");
			return status; 
		}

		*(PHANDLE)outbuf = FakeOpenProcess(*(PHANDLE)inbuf);
		if (*(PHANDLE)outbuf) {
			*writenlen = sizeof(HANDLE);
			return STATUS_SUCCESS;
		}
		return STATUS_UNSUCCESSFUL;
	}

	static auto CopyEprocess(PEPROCESS process) -> PEPROCESS {

		KAPC_STATE apc = { 0 };
		auto fProcess = ExAllocatePool(NonPagedPool, PAGE_SIZE);
		auto fakeCr3 = ExAllocatePool(NonPagedPool, PAGE_SIZE);

		if (fProcess == nullptr || fakeCr3 == nullptr || (UINT_PTR)fProcess & 0xfff || (UINT_PTR)fakeCr3 & 0xfff) {
			DbgPrint("failed to allocate mem\n");
			return nullptr;
		} 

		auto cprocess = IoGetCurrentProcess(); 

		memcpy(fProcess, PAGE_ALIGN(cprocess),PAGE_SIZE); //按页对齐，连对象头一起拷贝
		KeStackAttachProcess(process, &apc);

		auto gameCr3 = __readcr3();
		auto gameCr3Va = PaToVa(gameCr3 & ~0xfff); // 去掉 pcid
		memcpy(fakeCr3, gameCr3Va, PAGE_SIZE);
		KeUnstackDetachProcess(&apc);

		auto offset = (UINT_PTR)cprocess & 0xfff;
		*(PUINT_PTR)((UINT_PTR)fProcess + offset + 0x28) = VaToPa(fakeCr3);// 替换 Cr3 ， CR3 的值存储在 KPROCESS 的 DirectoryTableBase 成员中 。将 fProcess（一个指向进程结构体的指针）转换为 UINT_PTR，即一个无符号整数类型，用于指针运算。
		auto ObHeaderCookie = GetObHeaderCookie();
		auto TypeIndex = (unsigned char)ObHeaderCookie ^ (unsigned char)(((UINT_PTR)fProcess + offset - 0x30) >> 8) ^ 7;

		*(unsigned char*)((UINT_PTR)fProcess + offset - 0x18) = TypeIndex;


		//VAD: _EPROCESS->VadRoot
		*(PUINT_PTR)((UINT_PTR)fProcess + offset + GetVadOffset()) = *(PUINT_PTR)((UINT_PTR)process + GetVadOffset());
			
		//PEB 
		*(PUINT_PTR)((UINT_PTR)fProcess + offset + GetPebOffset()) = *(PUINT_PTR)((UINT_PTR)process + GetPebOffset());
		return (PEPROCESS)((UINT_PTR)fProcess + offset);
	}


	auto FakeOpenProcess(HANDLE pid) -> HANDLE {
		PEPROCESS Process;
		PEPROCESS FakeProcess;
		HANDLE hProcess; 
		NTSTATUS state;

		state = PsLookupProcessByProcessId(pid, &Process);

		if (!NT_SUCCESS(state)) {
			DbgPrint("failed to look up process\n");
			return nullptr;
		}

		ObDereferenceObject(Process);

		FakeProcess = CopyEprocess(Process);

		if (FakeProcess == nullptr) {
			DbgPrint("failed to Fake Process\n");
			return nullptr;
		}

		state = ObOpenObjectByPointer(FakeProcess, 0, 0, PROCESS_ALL_ACCESS, *PsProcessType, KernelMode, &hProcess); // 这个函数能根据传进来的句柄创建一个对象

		if (!NT_SUCCESS(state)) {
			DbgPrint("failed to open process\n");
			return nullptr;
		}

		return hProcess;
	}
}