#include "BaseSimplePipeThread.h"
#include <process.h>
#include "GPPMagicGlassAPI.h"

using namespace std;

// NodeJS默认设置
#define PIPE_BUFSIZE 65536
#define PIPE_TIMEOUT 0

CBaseSimplePipeThread::CBaseSimplePipeThread(void)
    :m_hThreadPipeHandle(0),
    m_hThreadTaskHandle(0),
    m_bStop(FALSE)
{
    InitParams();
}

CBaseSimplePipeThread::~CBaseSimplePipeThread(void)
{
}

unsigned __stdcall CBaseSimplePipeThread::ServerPipeThreadProc(void* pParam)
{
    CBaseSimplePipeThread* lpSimplePipe = reinterpret_cast<CBaseSimplePipeThread*>(pParam);
    if (lpSimplePipe)
    {
        while (1)
        {
            // 获取pipe中的消息
            if (lpSimplePipe->m_bStop) {
                break;
            }

            lpSimplePipe->Execute();
        }

    }

    return 0;
}

unsigned __stdcall CBaseSimplePipeThread::ServerTaskThreadProc(void* pParam)
{
    CBaseSimplePipeThread* lpSimplePipe = reinterpret_cast<CBaseSimplePipeThread*>(pParam);
    if (lpSimplePipe)
    {
        while (1)
        {
            // 执行获取到的消息
            if (lpSimplePipe->m_Msges.size() > 0) {
                // 多线程同步
                MutexLocker lock(lpSimplePipe->mMsgesMutex);
                // 获取并执行消息
                vector<std::string>::iterator msg = lpSimplePipe->m_Msges.begin();
                const char* msgInfo = (*msg).c_str();
                char* tempStr = new char[strlen(msgInfo) + 1];
                strcpy_s(tempStr, strlen(msgInfo) + 1, msgInfo);
                lpSimplePipe->m_Msges.erase(msg);
                GPPMagicGlassAPI::ParseGPPRequest((CHAR*)tempStr);
                delete[] tempStr;
            }
            else {
                if (lpSimplePipe->m_bStop) {
                    break;
                }
                Sleep(1);
            }
        }

    }

    return 0;
}

void CBaseSimplePipeThread::Execute()
{
    std::wstring szPipeName = L"\\\\.\\pipe\\gamepp\\";
    szPipeName.append(m_szPipeName);

    // 尝试解决ConnectNamedPipe卡住
    DeleteFile(szPipeName.c_str());

    m_hPipe = CreateNamedPipe(szPipeName.c_str(),
        PIPE_ACCESS_DUPLEX,
        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
        PIPE_UNLIMITED_INSTANCES,
        PIPE_BUFSIZE,
        PIPE_BUFSIZE,
        PIPE_TIMEOUT,
        NULL);
    if (m_hPipe == INVALID_HANDLE_VALUE)
        return;

    while (1)
    {
        if (m_bStop) {
            break;
        }

        if (ConnectNamedPipe(m_hPipe, NULL)) {
            CHAR* chRequest = new CHAR[PIPE_BUFSIZE + 8];
            memset(chRequest, 0, PIPE_BUFSIZE + 8);
            DWORD cbBytesRead;
            std::string szInfo;
            while (ReadFile(m_hPipe, chRequest, PIPE_BUFSIZE, &cbBytesRead, NULL))
            {
                BOOL isComplete = FALSE;
                if (cbBytesRead <= PIPE_BUFSIZE) {
                    chRequest[cbBytesRead] = '\0';
                    isComplete = TRUE;
                }
                szInfo.append(chRequest, cbBytesRead);
                if (isComplete) {
                    break;
                }
                memset(chRequest, 0, PIPE_BUFSIZE + 8);
            }
            if (szInfo.length() > 0) {
                MutexLocker lock(mMsgesMutex);
                if (m_Msges.size() != 0 && ((m_Msges.size() + 1) == m_Msges.capacity())) {
                    m_Msges.reserve(m_Msges.capacity() * 2);
                }
                m_Msges.push_back(szInfo.c_str());
            }
            delete[] chRequest;
        }

        DisconnectNamedPipe(m_hPipe);
    }
}

void CBaseSimplePipeThread::InitParams()
{
    m_szPipeName = L"gpp_magicglass_srv_in";
    m_Msges.reserve(128);
}

void CBaseSimplePipeThread::StartServer()
{
    m_bStop = FALSE;
    m_hThreadPipeHandle = reinterpret_cast<HANDLE>(_beginthreadex(NULL, 0, ServerPipeThreadProc, (void*)this, 0, NULL));
    m_hThreadTaskHandle = reinterpret_cast<HANDLE>(_beginthreadex(NULL, 0, ServerTaskThreadProc, (void*)this, 0, NULL));
}

void CBaseSimplePipeThread::StopServer()
{
    m_bStop = TRUE;

    std::wstring szPipeName = L"\\\\.\\pipe\\gamepp\\";
    szPipeName.append(m_szPipeName);

    // 尝试解决ConnectNamedPipe卡住
    DeleteFile(szPipeName.c_str());

    if (m_hPipe)
    {
        CloseHandle(m_hPipe);
        m_hPipe = 0;
    }
    if (m_hThreadPipeHandle)
    {
        WaitForSingleObject(m_hThreadPipeHandle, 3000);
        m_hThreadPipeHandle = 0;
    }
    if (m_hThreadTaskHandle)
    {
        WaitForSingleObject(m_hThreadTaskHandle, 3000);
        m_hThreadTaskHandle = 0;
    }
}
