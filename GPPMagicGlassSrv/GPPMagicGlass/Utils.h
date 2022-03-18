#pragma once
#include <Windows.h>
#include <string>

struct AudioDeviceInfo
{
    uint32_t sampleRate;
    int channel;
    int bit;
    uint32_t bitRate;
};

namespace Utils
{
    std::wstring string2wstring(std::string str);
    std::string wstring2string(std::wstring wstr);

    std::string double2string(double value, int precision);

    BOOL KillProcessByName(std::wstring processName);

    /// <summary>
    /// 给GamePP服务端发送信息
    /// </summary>
    /// <param name="lpstr">信息</param>
    void SendMessageToGPP(const char* lpstr);

    HWND GetWindowHwndByPID(DWORD dwProcessID);
}
