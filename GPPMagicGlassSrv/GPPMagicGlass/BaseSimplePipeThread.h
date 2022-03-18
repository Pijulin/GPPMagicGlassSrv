#pragma once
#include <Windows.h>
#include <string>
#include <vector>
#include "Mutex.h"

class CBaseSimplePipeThread
{
public:
    CBaseSimplePipeThread(void);
    virtual ~CBaseSimplePipeThread(void);

private:
    HANDLE m_hPipe;
    HANDLE m_hThreadPipeHandle;
    HANDLE m_hThreadTaskHandle;

    BOOL m_bStop;
    std::wstring m_szPipeName;

    Mutex mMsgesMutex;
    std::vector<std::string> m_Msges;

    static unsigned __stdcall ServerPipeThreadProc(void* pParam);
    static unsigned __stdcall ServerTaskThreadProc(void* pParam);

    void Execute();

protected:
    virtual void InitParams();

public:
    void StartServer();
    void StopServer();
};
