#pragma once
#include <Windows.h>
#include <string>
#include "SingletonT.h"

using namespace std;

struct RunInfoData
{
    UINT pId;						    // 游戏进程PID
    int effectMode;					    // 0 FSR; 1 FSRDX10显卡兼容模式
    int captureMode;				    // 0 Graphics Capture; 1 Desktop Duplication; 2 GDI; 3 Dwm Shared Surface
    float cursorZoom;				    // 鼠标缩放负数和 0：和源原窗口相同，正数：缩放比例0.5; 0.75; 1.0; 1.25; 1.5; 2; 3
    int cursorInterpolationMode;	    // 0：最近邻，1：双线性
    unsigned long adapterLUID;	        // 请选择游戏运行使用的显卡的LUID
    int multiMonitor;				    // 0：最近 1：相交 2：所有
    int DisableWindowResizing;		    // 缩放时禁用窗口大小调整【YES/NO】
    int CropTitleBarOfUWP;			    // 裁剪UWP窗口的标题栏【YES/NO】
    int DisableLowLatency;			    // 允许额外的延迟以提高性能【YES/NO】
    int NoCursor;					    // 不绘制光标【YES/NO】	
    int AdjustCursorSpeed;			    // 缩放时调整光标速度【YES/NO】
    int ConfineCursorIn3DGames;		    // 在3D游戏中限制光标【YES/NO】
    int DisableDirectFlip;			    // 禁用DirectFlip【YES/NO】
    int SimulateExclusiveFullscreen;    // 模拟独占全屏
    int disableVsync;					// 关闭垂直同步【YES/NO】
};

class RenderThread:public CSingletonT<RenderThread>
{
public:
    ~RenderThread();

private:
    SINGLETON(RenderThread);
    RenderThread();

    HANDLE m_hRenderEvent;
    HANDLE m_hThreadHandle;

    BOOL m_bStop;

    BOOL m_bRunning;

    UINT m_uActionType;

    UINT m_uLogLevel;
    RunInfoData m_Params;

    static unsigned __stdcall RenderThreadProc(void* pParam);

public:
    void StartLoop();
    void StopLoop();

    BOOL GetRunningState() {
        return m_bRunning;
    }

    string GetGPUList();

    void InitApp();

    void SetLog(UINT logLevel);

    void RunParams(RunInfoData Data);

    void Exit();
};

#define MGR_RT RenderThread::GetInstance()
