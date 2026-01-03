#pragma once
#include"ia32/ia32.hpp"
#include<ntddk.h>

struct PAGE_TABLE
{
	struct
	{
		pte_64* Pte;
		pde_64* Pde;
		pdpte_64* Pdpte;
		pml4e_64* Pml4e;
	}Entry;
	ULONG64 VirtualAddress;
};