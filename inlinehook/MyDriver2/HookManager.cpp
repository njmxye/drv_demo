#include "HookManager.h"
#include"hde64.h"
HookManager* HookManager::mInstance;

#pragma warning (disable : 4838)
#pragma warning (disable : 4309)
#pragma warning (disable : 4244)
#pragma warning (disable : 6328)
#pragma warning (disable : 6066)
#pragma warning (disable : 4996)


bool HookManager::InstallInlinehook(__inout void** originAddr, void* hookAddr)
{
    static bool bFirst = true;
    if (bFirst) {
        mTrampLinePool = (char*)ExAllocatePoolWithTag(NonPagedPool, PAGE_SIZE * 4, 'Jmp'); // ExAllocatePool2 蓝屏！！！！！！！

        if (!mTrampLinePool) {
            DbgPrint("Error InstallInlinehook");
            return false;
        };

        RtlZeroMemory(mTrampLinePool, PAGE_SIZE * 4);   
        mPoolUSED = 0;
        bFirst = false;
 
    }

    if (mHookCount == MAX_HOOK_COUNT) {
        DbgPrint("FULL");
        return false;
    };

    const UINT32 trampLineByteCount = 20;
    const UINT32 fnBreakByteLeast = 12;

    /*
    push 0
    mov dword ptr ds : [rsp] , 0
    mov dword ptr ds : [rsp + 4] , 0
    */
    char TrampLineCode[trampLineByteCount] = { 
        0x6A,0x00 ,0x3E ,0xC7 ,0x04 ,0x24 ,0x00 ,0x00 ,0x00 ,
        0x00 ,0x3E ,0xC7 ,0x44 ,0x24 ,0x04 ,0x00 ,0x00 ,0x00 ,0x00 ,0xC3 };

    /*
        mov rax, 0 
        Jmp rax
    */
    char AbsoluteJmpCode[fnBreakByteLeast] = {
        0x48,0xB8,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0xFF,0xE0
    };



    char* curTrampLinePool = mTrampLinePool + mPoolUSED;
    char* startJmpAddr = (char*)*originAddr;  // 要HOOK函数的首地址
    UINT32 uBreakBytes = 0; 
    hde64s hdeinfo = { 0 };

    while (uBreakBytes < fnBreakByteLeast) {
        if (!hde64_disasm(startJmpAddr + uBreakBytes, &hdeinfo)) {
            DbgPrint("hde64_disasm error \n");
            return false;
        };
        uBreakBytes += hdeinfo.len;
    };

    *(PUINT32)&TrampLineCode[6] = (UINT32)((UINT64)(startJmpAddr + uBreakBytes) & 0xFFFFFFFF); // 取高位
    *(PUINT32)&TrampLineCode[15] = (UINT32)((UINT64)(startJmpAddr + uBreakBytes)>>32 & 0xFFFFFFFF); //取低位

    memcpy(curTrampLinePool, startJmpAddr, uBreakBytes); //保存原函数的 内容
    memcpy(curTrampLinePool + uBreakBytes, TrampLineCode, trampLineByteCount);  //return 语句


    *(void**)&AbsoluteJmpCode[2] = hookAddr; // 数组地址转位一级指针：数组本身就是地址，& 取一次值就变成了耳机指针， 在 * 取一次值
    REPROTECT_CONTEXT Content = { 0 };

    if (!NT_SUCCESS(MmLockVaForWrite(startJmpAddr, PAGE_SIZE, &Content))) {
        return false;
    }

    RtlCopyMemory(Content.Lockedva, AbsoluteJmpCode, fnBreakByteLeast);
    
    if (!NT_SUCCESS(MmUnlockVaForWrite(&Content))) {
        return false;
    }
    *originAddr = curTrampLinePool;
    mPoolUSED += (uBreakBytes + trampLineByteCount);

    return true;
}

bool HookManager::RemoveInlinehook(void* hookAddr)
{
    UNREFERENCED_PARAMETER(hookAddr);
    return false;
}

HookManager* HookManager::GetInstance()
{
    if (mInstance == nullptr) {
        mInstance = (HookManager*)ExAllocatePoolWithTag(NonPagedPool, sizeof(HookManager), 'test');
    }
    return mInstance;
}
