#pragma once
#include"structer.h"
#include"MDL.h"

class HookManager
{
	// µ¥ÀýÄ£Ê½
public: 
	bool InstallInlinehook(__inout void** originAddr, void* hookAddr );
	bool RemoveInlinehook(void* hookAddr);
	static HookManager* GetInstance();

	UINT32 mHookCount = 0; 

	HOOK_INFO mHookInfo[MAX_HOOK_COUNT] = { 0 };

	char* mTrampLinePool = 0;
	UINT32 mPoolUSED = 0;

	static HookManager* mInstance;
};

