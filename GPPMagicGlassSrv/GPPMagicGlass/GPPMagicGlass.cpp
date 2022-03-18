// GPPMagicGlass.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "BackgroundWindow.h"
#include <wchar.h>

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    CAIViewWindow bgw;
    if (bgw.IsMainUIExists())
        return -1;

    WCHAR szPath[MAX_PATH + 1] = { 0 };
    GetModuleFileName(NULL, szPath, MAX_PATH);
    ::PathRemoveFileSpec(szPath);
    ::SetCurrentDirectory(szPath);

    bgw.InitDPI();
    bgw.CreateMainWindow(hPrevInstance);
    bgw.MessageLoop();

    return 0;
}
