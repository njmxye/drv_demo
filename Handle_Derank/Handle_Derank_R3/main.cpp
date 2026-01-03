#include <Windows.h>
#include <iostream>

#define DEVICE_LINK_NAME "\\\\.\\Handle_Derank"

void main() {
	HANDLE hdevice = NULL;
	hdevice = CreateFileA(DEVICE_LINK_NAME, GENERIC_READ | GENERIC_WRITE,0 , NULL, OPEN_EXISTING, NULL, NULL);

	if (hdevice == (HANDLE)-1) {
		printf("Create fail\n");  
	}

	CloseHandle(hdevice);
}