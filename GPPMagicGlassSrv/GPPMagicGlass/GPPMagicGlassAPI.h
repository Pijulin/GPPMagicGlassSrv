#pragma once
#include <windows.h>

namespace GPPMagicGlassAPI
{
    /// <summary>
    /// 解析GPP发送过来的数据并根据数据进行执行
    /// </summary>
    /// <param name="Data">数据信息,格式：JSON字符串</param>
    void ParseGPPRequest(CHAR* Data);
};
