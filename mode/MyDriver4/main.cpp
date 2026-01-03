#include<ntifs.h>
#include<ntddk.h>

#pragma warning(disable:4201)
typedef struct _LDR_DATA_TABLE_ENTRY
{
    struct _LIST_ENTRY InLoadOrderLinks;                                    //0x0
    struct _LIST_ENTRY InMemoryOrderLinks;                                  //0x10
    struct _LIST_ENTRY InInitializationOrderLinks;                          //0x20
    VOID* DllBase;                                                          //0x30
    VOID* EntryPoint;                                                       //0x38
    ULONG SizeOfImage;                                                      //0x40
    struct _UNICODE_STRING FullDllName;                                     //0x48
    struct _UNICODE_STRING BaseDllName;                                     //0x58
    union
    {
        UCHAR FlagGroup[4];                                                 //0x68
        ULONG Flags;                                                        //0x68
        struct
        {
            ULONG PackagedBinary : 1;                                         //0x68
            ULONG MarkedForRemoval : 1;                                       //0x68
            ULONG ImageDll : 1;                                               //0x68
            ULONG LoadNotificationsSent : 1;                                  //0x68
            ULONG TelemetryEntryProcessed : 1;                                //0x68
            ULONG ProcessStaticImport : 1;                                    //0x68
            ULONG InLegacyLists : 1;                                          //0x68
            ULONG InIndexes : 1;                                              //0x68
            ULONG ShimDll : 1;                                                //0x68
            ULONG InExceptionTable : 1;                                       //0x68
            ULONG ReservedFlags1 : 2;                                         //0x68
            ULONG LoadInProgress : 1;                                         //0x68
            ULONG LoadConfigProcessed : 1;                                    //0x68
            ULONG EntryProcessed : 1;                                         //0x68
            ULONG ProtectDelayLoad : 1;                                       //0x68
            ULONG ReservedFlags3 : 2;                                         //0x68
            ULONG DontCallForThreads : 1;                                     //0x68
            ULONG ProcessAttachCalled : 1;                                    //0x68
            ULONG ProcessAttachFailed : 1;                                    //0x68
            ULONG CorDeferredValidate : 1;                                    //0x68
            ULONG CorImage : 1;                                               //0x68
            ULONG DontRelocate : 1;                                           //0x68
            ULONG CorILOnly : 1;                                              //0x68
            ULONG ChpeImage : 1;                                              //0x68
            ULONG ReservedFlags5 : 2;                                         //0x68
            ULONG Redirected : 1;                                             //0x68
            ULONG ReservedFlags6 : 2;                                         //0x68
            ULONG CompatDatabaseProcessed : 1;                                //0x68
        };
    };
    USHORT ObsoleteLoadCount;                                               //0x6c
    USHORT TlsIndex;                                                        //0x6e
    struct _LIST_ENTRY HashLinks;                                           //0x70
    ULONG TimeDateStamp;                                                    //0x80
    struct _ACTIVATION_CONTEXT* EntryPointActivationContext;                //0x88
    VOID* Lock;                                                             //0x90
    struct _LDR_DDAG_NODE* DdagNode;                                        //0x98
    struct _LIST_ENTRY NodeModuleLink;                                      //0xa0
    struct _LDRP_LOAD_CONTEXT* LoadContext;                                 //0xb0
    VOID* ParentDllBase;                                                    //0xb8
    VOID* SwitchBackContext;                                                //0xc0
    struct _RTL_BALANCED_NODE BaseAddressIndexNode;                         //0xc8
    struct _RTL_BALANCED_NODE MappingInfoIndexNode;                         //0xe0
    ULONGLONG OriginalBase;                                                 //0xf8
    union _LARGE_INTEGER LoadTime;                                          //0x100
    ULONG BaseNameHashValue;                                                //0x108
    enum _LDR_DLL_LOAD_REASON LoadReason;                                   //0x10c
    ULONG ImplicitPathOptions;                                              //0x110
    ULONG ReferenceCount;                                                   //0x114
    ULONG DependentLoadFlags;                                               //0x118
    UCHAR SigningLevel;                                                     //0x11c
}LDR_DATA_TABLE_ENTRY, *PLDR_DATA_TABLE_ENTRY;

void UnloadDriver(PDRIVER_OBJECT DriverObject) {
	UNREFERENCED_PARAMETER(DriverObject); 
}

UINT64 mGetModName(PDRIVER_OBJECT DriverObject, UNICODE_STRING moduleName) {
    PLDR_DATA_TABLE_ENTRY pldr = NULL;
    PLIST_ENTRY pListEntry = NULL; 
    PLIST_ENTRY pListCurrent = NULL;
    PLDR_DATA_TABLE_ENTRY pCurrentMod = NULL;

    pldr = (PLDR_DATA_TABLE_ENTRY) DriverObject->DriverSection;
    pListEntry = pldr->InLoadOrderLinks.Flink;
    pListCurrent = pListEntry->Flink;
     
    while (pListEntry != pListCurrent) {
        pCurrentMod = CONTAINING_RECORD(pListCurrent, LDR_DATA_TABLE_ENTRY, InLoadOrderLinks); //结构体成员的指针中获取整个结构体的指针

        if (pCurrentMod->BaseDllName.Buffer != nullptr) {

            if (RtlCompareUnicodeString(&pCurrentMod->BaseDllName, &moduleName, true) == 0) {
                DbgPrint("pCurrnetModule:%wZ, DllBase: %p \n", pCurrentMod->BaseDllName, pCurrentMod->DllBase);
                return (UINT64)pCurrentMod->DllBase;
            }


        }
        pListCurrent = pListCurrent->Flink;
    }

    return 0;
     
};

EXTERN_C NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegisterPath){
    RegisterPath;
	DriverObject->DriverUnload = UnloadDriver;
    UNICODE_STRING name = RTL_CONSTANT_STRING(L"ntoskrnl.exe");
    mGetModName(DriverObject, name);

	return STATUS_SUCCESS;
}