#pragma once
#include<Windows.h>
#include<winsvc.h>

BOOL LoadDriver(const char* lpszDrivername, const char* sysFileName);

BOOL UnloadDriver(const char* lpszDriverName);