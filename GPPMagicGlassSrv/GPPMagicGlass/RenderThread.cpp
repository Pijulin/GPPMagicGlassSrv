#include "RenderThread.h"
#include <process.h>
#include <json/json.h>
#include <LOG/Log.h>
#include "GPPMagicGlassModule.h"
#include "Utils.h"

RenderThread::RenderThread()
    :m_Params({ 0 }),
    m_hRenderEvent(0),
    m_hThreadHandle(0),
    m_uActionType(0),
    m_uLogLevel(6),
    m_bStop(FALSE),
    m_bRunning(FALSE)
{
    // 加载DLL
    GPPMagicGlassModule::LoadModule();
    
    // 创建EVENT
    m_hRenderEvent = CreateEvent(NULL, FALSE, FALSE, L"GPPMagicGlass_RenderThread");
}

RenderThread::~RenderThread()
{
    
}

unsigned __stdcall RenderThread::RenderThreadProc(void* pParam)
{
    RenderThread* lpRenderThread = reinterpret_cast<RenderThread*>(pParam);
    if (lpRenderThread)
    {
        while (true)
        {
            // 结束
            if (lpRenderThread->m_bStop) {
                break;
            }

            // 等待
            WaitForSingleObject(lpRenderThread->m_hRenderEvent, INFINITE);

            if (lpRenderThread->m_uActionType == 1) {
                // 初始化
                BOOL isInit = GPPMagicGlassModule::InitApp();
                if (!isInit) {
                    Json::Value result;
                    result["type"] = "INIT";
                    result["error"] = 1;

                    Json::StreamWriterBuilder writerBuilder;
                    string resultStr = Json::writeString(writerBuilder, result);
                    Utils::SendMessageToGPP(resultStr.c_str());
                }
                else {
                    Json::Value result;
                    result["type"] = "INIT";
                    result["error"] = 0;

                    Json::StreamWriterBuilder writerBuilder;
                    string resultStr = Json::writeString(writerBuilder, result);
                    Utils::SendMessageToGPP(resultStr.c_str());
                }
            }
            else if (lpRenderThread->m_uActionType == 2) {
                // 设置日志
                BOOL isSet = GPPMagicGlassModule::SetLog(lpRenderThread->m_uLogLevel);

                if (!isSet) {
                    Json::Value result;
                    result["type"] = "SETLOG";
                    result["error"] = 1;

                    Json::StreamWriterBuilder writerBuilder;
                    string resultStr = Json::writeString(writerBuilder, result);
                    Utils::SendMessageToGPP(resultStr.c_str());
                }
                else {
                    Json::Value result;
                    result["type"] = "SETLOG";
                    result["error"] = 0;

                    Json::StreamWriterBuilder writerBuilder;
                    string resultStr = Json::writeString(writerBuilder, result);
                    Utils::SendMessageToGPP(resultStr.c_str());
                }
            }
            else if (lpRenderThread->m_uActionType == 3) {
                // RUN
                lpRenderThread->m_bRunning = TRUE;

                string resultMsg = GPPMagicGlassModule::Run(
                    lpRenderThread->m_Params.pId,
                    lpRenderThread->m_Params.effectMode,
                    lpRenderThread->m_Params.captureMode,
                    lpRenderThread->m_Params.cursorZoom,
                    lpRenderThread->m_Params.cursorInterpolationMode,
                    lpRenderThread->m_Params.adapterLUID,
                    lpRenderThread->m_Params.multiMonitor,
                    lpRenderThread->m_Params.DisableWindowResizing,
                    lpRenderThread->m_Params.CropTitleBarOfUWP,
                    lpRenderThread->m_Params.DisableLowLatency,
                    lpRenderThread->m_Params.NoCursor,
                    lpRenderThread->m_Params.AdjustCursorSpeed,
                    lpRenderThread->m_Params.ConfineCursorIn3DGames,
                    lpRenderThread->m_Params.DisableDirectFlip,
                    lpRenderThread->m_Params.SimulateExclusiveFullscreen,
                    lpRenderThread->m_Params.disableVsync
                );

                if (resultMsg.length() > 0) {
                    // 出现ERROR
                    Json::Value result;
                    result["type"] = "RUN";
                    result["msg"] = resultMsg;
                    result["error"] = 1;

                    Json::StreamWriterBuilder writerBuilder;
                    string resultStr = Json::writeString(writerBuilder, result);
                    Utils::SendMessageToGPP(resultStr.c_str());
                }

                lpRenderThread->m_bRunning = FALSE;
            }

            // 重置状态
            ResetEvent(lpRenderThread->m_hRenderEvent);
        }

    }

    return 0;
}

void RenderThread::StartLoop()
{
    m_bStop = FALSE;
    m_hThreadHandle = reinterpret_cast<HANDLE>(_beginthreadex(NULL, 0, RenderThreadProc, (void*)this, 0, NULL));
}

void RenderThread::StopLoop()
{
    m_bStop = TRUE;

    Exit();

    if (m_hThreadHandle)
    {
        WaitForSingleObject(m_hThreadHandle, 3000);
        m_hThreadHandle = 0;
    }
}

string RenderThread::GetGPUList() {
    // GetGPUList 不需要保证线程
    return GPPMagicGlassModule::GetGPUList();
}

void RenderThread::InitApp() {
    // 需要保证与RUN、SetLog在同一线程
    // Run是阻塞的
    m_uActionType = 1; // 初始化
    // 激活状态
    SetEvent(m_hRenderEvent);
}

void RenderThread::SetLog(UINT logLevel) {
    // 需要保证与InitApp、Run在同一线程
    // Run是阻塞的
    m_uActionType = 2; // 设置日志模式
    m_uLogLevel = logLevel;
    // 激活状态
    SetEvent(m_hRenderEvent);
}

void RenderThread::RunParams(RunInfoData data)
{
    // 需要保证与InitApp、SetLog在同一线程
    // Run是阻塞的
    m_Params = data;
    m_uActionType = 3; // Run
    // 激活状态
    SetEvent(m_hRenderEvent);
}

void RenderThread::Exit()
{
    // CLOSE不需要保证线程
    GPPMagicGlassModule::Close();
}
