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
    // ����DLL
    GPPMagicGlassModule::LoadModule();
    
    // ����EVENT
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
            // ����
            if (lpRenderThread->m_bStop) {
                break;
            }

            // �ȴ�
            WaitForSingleObject(lpRenderThread->m_hRenderEvent, INFINITE);

            if (lpRenderThread->m_uActionType == 1) {
                // ��ʼ��
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
                // ������־
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
                    // ����ERROR
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

            // ����״̬
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
    // GetGPUList ����Ҫ��֤�߳�
    return GPPMagicGlassModule::GetGPUList();
}

void RenderThread::InitApp() {
    // ��Ҫ��֤��RUN��SetLog��ͬһ�߳�
    // Run��������
    m_uActionType = 1; // ��ʼ��
    // ����״̬
    SetEvent(m_hRenderEvent);
}

void RenderThread::SetLog(UINT logLevel) {
    // ��Ҫ��֤��InitApp��Run��ͬһ�߳�
    // Run��������
    m_uActionType = 2; // ������־ģʽ
    m_uLogLevel = logLevel;
    // ����״̬
    SetEvent(m_hRenderEvent);
}

void RenderThread::RunParams(RunInfoData data)
{
    // ��Ҫ��֤��InitApp��SetLog��ͬһ�߳�
    // Run��������
    m_Params = data;
    m_uActionType = 3; // Run
    // ����״̬
    SetEvent(m_hRenderEvent);
}

void RenderThread::Exit()
{
    // CLOSE����Ҫ��֤�߳�
    GPPMagicGlassModule::Close();
}
