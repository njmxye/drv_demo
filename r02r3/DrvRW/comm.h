#pragma once
#include <ntifs.h>
#include <fltKernel.h>
#include  <dontuse.h>
#include <ntstrsafe.h>

// include fltmgr.lib
namespace common {
	typedef NTSTATUS(*fnCommoFunc)(void* inbuf/*输入缓冲区*/, ULONG inlen/*输出缓冲区*/, void* outbuf, ULONG outlen, PULONG writenlen /*实际通讯字节*/);

	inline fnCommoFunc CommonFunc(nullptr);

	inline PFLT_FILTER fltFilter(nullptr);
	inline PFLT_PORT drvPort(nullptr);
	inline PFLT_PORT ClientPort(nullptr);

	auto createCommonPort(PDRIVER_OBJECT driverObject, PUNICODE_STRING RegisterPath, fnCommoFunc dunc) -> NTSTATUS;
}