#pragma once
#include <ntifs.h>
#include <ntddk.h>
#include <intrin.h>


namespace rw {

	auto commonIO(void* inbuf, ULONG inlen, void* outbuf, ULONG outlen, PULONG writenlen) -> NTSTATUS;
	auto FakeOpenProcess(HANDLE pid) -> HANDLE;
}
