#pragma once
#include<ntifs.h>
#include<ntddk.h>

#define MAX_HOOK_COUNT 10

typedef struct _HOOK_INFO {
	char originBytes[14];
	void* originAddr;
	

}HOOK_INFO, *PHOOK_INFO;