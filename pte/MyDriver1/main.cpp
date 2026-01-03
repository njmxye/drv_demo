#include<ntifs.h>
#include<ntddk.h>
#include<intrin.h>
#include"ia32/ia32.hpp" 
#include"struct.h"
/* git config --global --replace - all http.proxy http ://192.168.0.112:8889 */
#pragma warning(disable:4389)
void* GetPteBase() {
	cr3 CR3;
	PHYSICAL_ADDRESS cr3_pa = { 0 }; 
	CR3.flags =  __readcr3();
	cr3_pa.QuadPart = CR3.address_of_page_directory * PAGE_SIZE; 
	PULONG64 cr3_va  = (PULONG64) MmGetVirtualForPhysical(cr3_pa);

	UINT64 nCount = 0;
	while ((*cr3_va & 0x000FFFFFFFFFF000) != cr3_pa.QuadPart) {
		if (++nCount >= 512) {
			return nullptr;
		}
		cr3_va++;
	} 
	return (void*)(0xffff000000000000 | (nCount << 39));
}

bool GetPageTable(PAGE_TABLE& table) {
	ULONG64 PteBase = 0;
	ULONG64 pdeBase = 0;
	ULONG64 pdpteBase = 0;
	ULONG64 pml4eBase = 0;

	PteBase = (ULONG64)GetPteBase();
	DbgPrint("PteBase :%p\n", PteBase);

	if (PteBase == NULL) return false; 
	pdeBase = (((PteBase & 0xffffffffffff)>>12)<< 3)+ PteBase;
	pdpteBase = (((pdeBase & 0xffffffffffff) >> 12) << 3) + PteBase;
	pml4eBase = (((pdpteBase & 0xffffffffffff) >> 12) << 3) + PteBase;

	table.Entry.Pte		= (pte_64*)((((table.VirtualAddress & 0xffffffffffff) >> 12) << 3) + PteBase);
	table.Entry.Pde		= (pde_64*)((((table.VirtualAddress & 0xffffffffffff) >> 21) << 3) + pdeBase);
	table.Entry.Pdpte  = (pdpte_64*)((((table.VirtualAddress & 0xffffffffffff) >> 30) << 3) + pdpteBase);
	table.Entry.Pml4e   = (pml4e_64*)((((table.VirtualAddress & 0xffffffffffff) >> 39) << 3) + pml4eBase);
	
	return true;
}	

void DriverUnload(PDRIVER_OBJECT DriverObject) {
	DriverObject;
}

EXTERN_C NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PLSA_UNICODE_STRING RegistryPath) {
	//DbgPrint("DriverObject:%p\n", DriverObject);
	//DbgPrint("RegistryPath :%wz\n", RegistryPath);
	//DbgBreakPoint();
	//PVOID64 PteBase = GetPteBase();
	//DbgPrint("PteBase :%p\n", PteBase);
	RegistryPath; DriverObject;
	DriverObject->DriverUnload = DriverUnload;

	PAGE_TABLE pageTable{ 0 };
	pageTable.VirtualAddress = (ULONG64)NtOpenProcess;
	DbgPrint("NtOpenProcess: %p\n", NtOpenProcess);
	GetPageTable(pageTable);
	DbgPrint("pml4e :%p pdpte :%p pde :%p pte :%p ", \
		pageTable.Entry.Pml4e, pageTable.Entry.Pdpte, pageTable.Entry.Pde, pageTable.Entry.Pte);


	return  STATUS_SUCCESS;
} 