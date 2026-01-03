// Rw3.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include <Windows.h>
#include <iostream>
#include <fltUser.h>

#pragma comment(lib,"fltlib.lib")
#pragma comment(lib,"fltmgr.lib")

HANDLE  port = nullptr;
int main()
{
    auto bRet = 0;
    bRet = FilterConnectCommunicationPort(L"\\mf", 0, 0, 0, 0, &port);
    if (port == INVALID_HANDLE_VALUE) {
        std::cout << "fail to create handle\n";
    }

    HANDLE pid = (HANDLE)1;
    HANDLE fprocess = nullptr;
    ULONG uRet = 0;
    FilterSendMessage(port, &pid, sizeof(pid), &fprocess, sizeof(fprocess), &uRet);
    std::cout << "Hello World!\n";
}

