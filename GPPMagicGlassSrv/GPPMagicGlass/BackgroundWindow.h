#pragma once
#include <string>
#include <atlstr.h>
#include "BaseSimplePipeThread.h"
#include "RenderThread.h"

//设置主界面
class CAIViewWindow
{
public:
    CAIViewWindow();
    ~CAIViewWindow();
    void InitDPI();

public:
    BOOL CreateMainWindow(HINSTANCE hInstance);
    ATOM MyRegisterClass(HINSTANCE hInstance);

    static LRESULT CALLBACK OnMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    static void MessageLoop();
    void InitWindow();
    void SetInstance(HINSTANCE hInst);

    BOOL IsMainUIExists();
    void InitThreads();

private:
    PTP_POOL mThreadPool;
    TP_CALLBACK_ENVIRON mTce;

private:
    HWND m_hWnd;         //窗体句柄
    HINSTANCE m_hInstance;
    static CAIViewWindow* m_staticSelf;
    CBaseSimplePipeThread* m_pipeThread;
};

void DetectServiceIsQuitThread(LPVOID lpParam);
