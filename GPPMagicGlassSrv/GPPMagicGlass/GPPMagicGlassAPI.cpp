#include "GPPMagicGlassAPI.h"
#include <process.h>
#include <json/json.h>
#include <LOG/Log.h>
#include "Utils.h"
#include "RenderThread.h"

using namespace std;
using namespace Utils;

void GPPMagicGlassAPI::ParseGPPRequest(CHAR* Data) {
	try
	{
        string rawJson = Data;
        const auto rawJsonLength = static_cast<int>(rawJson.length());
        Json::Value root;
        JSONCPP_STRING errs;

        Json::CharReaderBuilder readerBuilder;
        const std::unique_ptr<Json::CharReader> reader(readerBuilder.newCharReader());
        bool res = reader->parse(rawJson.c_str(), rawJson.c_str() + rawJsonLength, &root, &errs);
        if (!res || !errs.empty()) {
            // JSON解析失败
            Json::Value result;
            result["type"] = "MAGICGLASSSRV";
            result["code"] = 1;

            Json::StreamWriterBuilder writerBuilder;
            string resultStr = Json::writeString(writerBuilder, result);
            SendMessageToGPP(resultStr.c_str());
        }
        else {
            // 解析成功
            if (root.isObject()) {
                string type = root.get("type", "UNKNOWN").asString();
                if (type.compare("GPUDETECT") == 0) {
                    // 获取显卡列表
                    string list = MGR_RT->GetGPUList();
                    if (list.length() == 0) {
                        // 获取失败
                        Json::Value result;
                        result["type"] = "GPUDETECT";
                        result["list"] = list;
                        result["error"] = 1;

                        Json::StreamWriterBuilder writerBuilder;
                        string resultStr = Json::writeString(writerBuilder, result);
                        SendMessageToGPP(resultStr.c_str());
                    }
                    else {
                        // 返回
                        Json::Value result;
                        result["type"] = "GPUDETECT";
                        result["list"] = list;
                        result["error"] = 0;

                        Json::StreamWriterBuilder writerBuilder;
                        string resultStr = Json::writeString(writerBuilder, result);
                        SendMessageToGPP(resultStr.c_str());
                    }
                }
                else if (type.compare("INIT") == 0) {
                    // 初始化
                    MGR_RT->InitApp();
                }
                else if (type.compare("SETLOG") == 0) {
                    // 设置日志
                    UINT logLevel = root.get("logLevel", 6).asUInt();
                    MGR_RT->SetLog(logLevel);
                }
                else if (type.compare("RUN") == 0) {
                    // 开启
                    UINT pId = root.get("pId", 0).asUInt();
                    int effectMode = root.get("effectMode", 0).asInt();
                    int captureMode = root.get("captureMode", 0).asInt();
                    float cursorZoom = root.get("cursorZoom", 1.0).asFloat();
                    int cursorInterpolationMode = root.get("cursorInterpolationMode", 0).asInt();
                    unsigned long adapterLUID = root.get("adapterLUID", 0).asUInt64();
                    int multiMonitor = root.get("multiMonitor", 0).asInt();
                    int DisableWindowResizing = root.get("DisableWindowResizing", 0).asInt();
                    int CropTitleBarOfUWP = root.get("CropTitleBarOfUWP", 0).asInt();
                    int DisableLowLatency = root.get("DisableLowLatency", 0).asInt();
                    int NoCursor = root.get("NoCursor", 0).asInt();
                    int AdjustCursorSpeed = root.get("AdjustCursorSpeed", 0).asInt();
                    int ConfineCursorIn3DGames = root.get("ConfineCursorIn3DGames", 0).asInt();
                    int DisableDirectFlip = root.get("DisableDirectFlip", 0).asInt();
                    int SimulateExclusiveFullscreen = root.get("SimulateExclusiveFullscreen", 0).asInt();
                    int disableVsync = root.get("disableVsync", 0).asInt();

                    RunInfoData data = { 0 };
                    data.pId = pId;
                    data.effectMode = effectMode;
                    data.captureMode = captureMode;
                    data.cursorZoom = cursorZoom;
                    data.cursorInterpolationMode = cursorInterpolationMode;
                    data.adapterLUID = adapterLUID;
                    data.multiMonitor = multiMonitor;
                    data.DisableWindowResizing = DisableWindowResizing;
                    data.CropTitleBarOfUWP = CropTitleBarOfUWP;
                    data.DisableLowLatency = DisableLowLatency;
                    data.NoCursor = NoCursor;
                    data.AdjustCursorSpeed = AdjustCursorSpeed;
                    data.ConfineCursorIn3DGames = ConfineCursorIn3DGames;
                    data.DisableDirectFlip = DisableDirectFlip;
                    data.SimulateExclusiveFullscreen = SimulateExclusiveFullscreen;
                    data.disableVsync = disableVsync;

                    MGR_RT->RunParams(data);
                }
                else if (type.compare("STATE") == 0) {
                    // 初始化
                    BOOL state = MGR_RT->GetRunningState();

                    Json::Value result;
                    result["type"] = "STATE";
                    result["state"] = state;

                    Json::StreamWriterBuilder writerBuilder;
                    string resultStr = Json::writeString(writerBuilder, result);
                    SendMessageToGPP(resultStr.c_str());
                }
                else if (type.compare("CLOSE") == 0) {
                    MGR_RT->Exit();

                    Json::Value result;
                    result["type"] = "CLOSE";
                    result["error"] = 0;

                    Json::StreamWriterBuilder writerBuilder;
                    string resultStr = Json::writeString(writerBuilder, result);
                    SendMessageToGPP(resultStr.c_str());
                }
            }
            else {
                // 非法格式
                Json::Value result;
                result["type"] = "MAGICGLASSSRV";
                result["code"] = 2;

                Json::StreamWriterBuilder writerBuilder;
                string resultStr = Json::writeString(writerBuilder, result);
                SendMessageToGPP(resultStr.c_str());
            }
        }
	}
    catch (const std::exception&)
    {
        Json::Value result;
        result["type"] = "MAGICGLASSSRV";
        result["code"] = 1;

        Json::StreamWriterBuilder writerBuilder;
        string resultStr = Json::writeString(writerBuilder, result);
        SendMessageToGPP(resultStr.c_str());
    }
}