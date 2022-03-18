
#include "BackgroundWindow.h"
#include <XHelper/process.h>

#define IDI_GAMEPPOBSSVR    118
#define WINDOW_INFOR_CLASS  _T("GAMEPP_MAGICGLASS_WINDOW")
#define WINDOW_NAME         _T("GAMEPPMAGICGLASSSRV")

using namespace std;

CAIViewWindow* CAIViewWindow::m_staticSelf = NULL;

CAIViewWindow::CAIViewWindow()
    : m_hWnd(NULL)
{
    m_hInstance = NULL;
    m_pipeThread = NULL;
    mThreadPool = NULL;
    mTce = {};
    m_staticSelf = this;

}

CAIViewWindow::~CAIViewWindow()
{

}

void CAIViewWindow::InitDPI()
{
    m_pipeThread = new CBaseSimplePipeThread();
    m_pipeThread->StartServer();

    MGR_RT->StartLoop();
}

BOOL CAIViewWindow::CreateMainWindow(HINSTANCE hInstance)
{
    m_hInstance = hInstance;
    MyRegisterClass(hInstance);

    //设置窗口为无边框，无菜单.
    const DWORD dwStyle = WS_OVERLAPPED | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_POPUP | WS_VISIBLE | DS_SETFONT | WS_MINIMIZEBOX;
    //
    const DWORD dwStyleEx = WS_EX_TOOLWINDOW;
    m_hWnd = CreateWindowExW(
        dwStyleEx,
        WINDOW_INFOR_CLASS,
        WINDOW_NAME,
        dwStyle,
        -1000,
        -1000,
        1,
        1,
        nullptr,
        nullptr,
        hInstance,
        nullptr);

    if (!m_hWnd)
    {
        return FALSE;
    }
    ShowWindow(m_hWnd, SW_HIDE);
    UpdateWindow(m_hWnd);

    InitWindow();

    return TRUE;
}

ATOM CAIViewWindow::MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = OnMessage;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = NULL;
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDI_GAMEPPOBSSVR);
    wcex.lpszClassName = WINDOW_INFOR_CLASS;
    wcex.hIconSm = NULL;

    return RegisterClassExW(&wcex);
}

LRESULT CALLBACK CAIViewWindow::OnMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps = {};
        HDC hdc = BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_TIMER:
        break;
    case WM_COPYDATA:
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

void CAIViewWindow::MessageLoop()
{
    // 主消息循环: 
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

void CAIViewWindow::InitWindow()
{
    InitThreads();
    HANDLE hDetectThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)DetectServiceIsQuitThread, this, 0, NULL);
    if (hDetectThread) {
        CloseHandle(hDetectThread);
    }
}

void CAIViewWindow::SetInstance(HINSTANCE hInst)
{
    m_hInstance = hInst;
}

BOOL CAIViewWindow::IsMainUIExists()
{
    HANDLE  hMutex = CreateMutex(NULL, FALSE, L"GAMEPP_MAGICGLASS_MUTEX");
    int iError = GetLastError();
    if (iError == ERROR_ALREADY_EXISTS)
    {
        return TRUE;
    }

    return FALSE;
}

void DetectServiceIsQuitThread(LPVOID lpParam)
{
    while (XProcess::IsProcessExists(L"GamePP.exe"))
    {
        Sleep(2 * 1000);
    }

    TerminateProcess(GetCurrentProcess(), 0);
}

void CAIViewWindow::InitThreads()
{
    mThreadPool = CreateThreadpool(NULL);

    if (mThreadPool)
    {
        //设置线程池最大最小的线程数量
        SetThreadpoolThreadMinimum(mThreadPool, 1);
        SetThreadpoolThreadMaximum(mThreadPool, 1);

        //初始化线程池环境变量
        InitializeThreadpoolEnvironment(&mTce);

        //为线程池设置线程池环境变量
        SetThreadpoolCallbackPool(&mTce, mThreadPool);
    }
}
