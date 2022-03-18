#include "Utils.h"
#include <LOG/Log.h>
#include <vector>
#include <sstream>
#include <iomanip>
#include <Windows.h>
#include <TlHelp32.h>

//将string转换成wstring  
std::wstring Utils::string2wstring(std::string str)
{
    std::wstring result;
    //获取缓冲区大小，并申请空间，缓冲区大小按字符计算  
    int len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), NULL, 0);
    TCHAR* buffer = new TCHAR[len + 1];
    //多字节编码转换成宽字节编码  
    MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), buffer, len);
    buffer[len] = '\0';             //添加字符串结尾  
    //删除缓冲区并返回值  
    result.append(buffer);
    delete[] buffer;
    return result;
}

//将wstring转换成string  
std::string Utils::wstring2string(std::wstring wstr)
{
    std::string result;
    //获取缓冲区大小，并申请空间，缓冲区大小事按字节计算的  
    int len = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), NULL, 0, NULL, NULL);
    char* buffer = new char[len + 1];
    //宽字节编码转换成多字节编码  
    WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), buffer, len, NULL, NULL);
    buffer[len] = '\0';
    //删除缓冲区并返回值  
    result.append(buffer);
    delete[] buffer;
    return result;
}

BOOL Utils::KillProcessByName(std::wstring processName) {
    
    unsigned int pid = -1;
    BOOL retval = TRUE;

    if (processName.empty())
        return FALSE;

    DWORD dwRet = 0;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32 processInfo;
    processInfo.dwSize = sizeof(PROCESSENTRY32);
    int flag = Process32First(hSnapshot, &processInfo);

    // Find the process with name as same as processName
    while (flag != 0)
    {
        if (wcscmp(processInfo.szExeFile, processName.c_str()) == 0) {
            // Terminate the process.
            pid = processInfo.th32ProcessID;
            HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, TRUE, pid);
            BOOL isKill = TerminateProcess(hProcess, 0);
            if (!isKill) {
                retval = FALSE;
            } else {
                break;
            }
        }

        flag = Process32Next(hSnapshot, &processInfo);
    }

    CloseHandle(hSnapshot);

    if (pid == -1)
        return FALSE;

    return retval;
}

void Utils::SendMessageToGPP(const char* lpstr)
{
    HANDLE hPipe;
    DWORD dwWritten;

    std::wstring szPipeName = L"\\\\.\\pipe\\gamepp\\gpp_magicglass_srv_out";

    if (!WaitNamedPipe(szPipeName.c_str(), 3000)) {
        LOGINFO(L"WaitNamedPipe Failed : %d", GetLastError());
        return;
    }

    hPipe = CreateFile(
        szPipeName.c_str(),
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        0,
        NULL);

    if (hPipe != INVALID_HANDLE_VALUE)
    {
        WriteFile(hPipe,
            lpstr,
            strlen(lpstr),
            &dwWritten,
            NULL);

        CloseHandle(hPipe);
        LOGINFO(L"WriteFile CloseHandle");
    }
    else {
        LOGINFO(L"CreateFile Error : %d", GetLastError());
    }
}

std::string Utils::double2string(double value, int precision)
{
    std::stringstream ss;
    ss.setf(std::ios::fixed);
    ss.precision(precision);
    ss << value;
    return ss.str();
}

///< 枚举窗口参数
typedef struct
{
    HWND hwndWindow; // 窗口句柄
    DWORD dwProcessID; // 进程ID
}EnumWindowsArg;

///< 枚举窗口回调函数
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
    EnumWindowsArg* pArg = (EnumWindowsArg*)lParam;
    DWORD dwProcessID = 0;
    // 通过窗口句柄取得进程ID
    ::GetWindowThreadProcessId(hwnd, &dwProcessID);
    if (dwProcessID == pArg->dwProcessID)
    {
        pArg->hwndWindow = hwnd;
        // 找到了返回FALSE
        return FALSE;
    }
    // 没找到，继续找，返回TRUE
    return TRUE;
}

///< 通过进程ID获取窗口句柄
HWND Utils::GetWindowHwndByPID(DWORD dwProcessID)
{
    HWND hwndRet = NULL;
    EnumWindowsArg ewa;
    ewa.dwProcessID = dwProcessID;
    ewa.hwndWindow = NULL;
    EnumWindows(EnumWindowsProc, (LPARAM)&ewa);
    if (ewa.hwndWindow)
    {
        hwndRet = ewa.hwndWindow;
    }
    return hwndRet;
}
