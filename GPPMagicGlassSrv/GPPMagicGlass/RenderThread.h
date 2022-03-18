#pragma once
#include <Windows.h>
#include <string>
#include "SingletonT.h"

using namespace std;

struct RunInfoData
{
    UINT pId;						    // ��Ϸ����PID
    int effectMode;					    // 0 FSR; 1 FSRDX10�Կ�����ģʽ
    int captureMode;				    // 0 Graphics Capture; 1 Desktop Duplication; 2 GDI; 3 Dwm Shared Surface
    float cursorZoom;				    // ������Ÿ����� 0����Դԭ������ͬ�����������ű���0.5; 0.75; 1.0; 1.25; 1.5; 2; 3
    int cursorInterpolationMode;	    // 0������ڣ�1��˫����
    unsigned long adapterLUID;	        // ��ѡ����Ϸ����ʹ�õ��Կ���LUID
    int multiMonitor;				    // 0����� 1���ཻ 2������
    int DisableWindowResizing;		    // ����ʱ���ô��ڴ�С������YES/NO��
    int CropTitleBarOfUWP;			    // �ü�UWP���ڵı�������YES/NO��
    int DisableLowLatency;			    // ���������ӳ���������ܡ�YES/NO��
    int NoCursor;					    // �����ƹ�꡾YES/NO��	
    int AdjustCursorSpeed;			    // ����ʱ��������ٶȡ�YES/NO��
    int ConfineCursorIn3DGames;		    // ��3D��Ϸ�����ƹ�꡾YES/NO��
    int DisableDirectFlip;			    // ����DirectFlip��YES/NO��
    int SimulateExclusiveFullscreen;    // ģ���ռȫ��
    int disableVsync;					// �رմ�ֱͬ����YES/NO��
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
