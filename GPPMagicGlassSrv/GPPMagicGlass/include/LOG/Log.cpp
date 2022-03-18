#include "Log.h"
#include <tchar.h>
#include <strsafe.h>
#include "SmartLogger.h"

CSmartLogger g_SmartLogger;

#ifdef HEAP_MAKE_TAG_FLAGS
#undef HEAP_MAKE_TAG_FLAGS
#endif

BOOL g_bEnableFileLog = FALSE;
BOOL g_bEnableExtFileLog = FALSE;
BOOL g_bEnableDbgView = FALSE;

BOOL g_bEnableFileLog1 = FALSE;

//FORMAT  LOG_LEVEL File:Line INFO
#define FILE_LOG_FORMAT_A "  %s %6d  %20s:%5d  %64s  %s"
#define FILE_LOG_FORMAT_W L"  %ws %6d  %20ws:%5d  %64ws  %ws"

#define DBGVIEW_LOG_FORMAT_A "  %s %6d  %20s:%5d    %s"
#define DBGVIEW_LOG_FORMAT_W L"  %ws %6d  %20ws:%5d    %ws"

#ifdef LOG_TO_FILE

VOID _cdecl LogExtW(BOOL bLog, LOG_LEVEL llLevel, LPCWSTR lpszCppFileName, DWORD ThreadId, DWORD dwLine, LPCWSTR lpszFunctionName, LPCWSTR lpszFormat, ...)
{
	if (bLog)
	{
		va_list args;
		va_start(args, lpszFormat);

		WCHAR szBuffer[4096] = { 0 };
		WCHAR szOutText[4096 + 20] = { 0 };

		HRESULT hr = StringCbVPrintfW(szBuffer, sizeof(szBuffer), lpszFormat, args);
		if (SUCCEEDED(hr))
		{
			switch (llLevel)
			{
			default:
				StringCbPrintfW(szOutText, sizeof(szOutText), FILE_LOG_FORMAT_W, L"INFO ", ThreadId, lpszCppFileName, dwLine, lpszFunctionName, szBuffer);
				return;

			case LOG_LEVEL_ERROR:
				StringCbPrintfW(szOutText, sizeof(szOutText), FILE_LOG_FORMAT_W, L"ERROR ", ThreadId, lpszCppFileName, dwLine, lpszFunctionName, szBuffer);
				break;

			case LOG_LEVEL_FATAL:
				StringCbPrintfW(szOutText, sizeof(szOutText), FILE_LOG_FORMAT_W, L"FATAL ", ThreadId, lpszCppFileName, dwLine, lpszFunctionName, szBuffer);
				break;
			}

			g_SmartLogger.AddLog(szOutText);

			WriteTraceToFile();
		}
	}
}

VOID _cdecl LogW(BOOL bLog, LOG_LEVEL llLevel, LPCWSTR lpszCppFileName, DWORD ThreadId, DWORD dwLine, LPCWSTR lpszFunctionName, LPCWSTR lpszFormat, ...)
{

	if (bLog)
	{
		va_list args;
		va_start(args, lpszFormat);

		WCHAR szBuffer[0x4096 * 2] = { 0 };
		WCHAR szOutText[0x4096 * 2 + 20] = { 0 };

		HRESULT hr = StringCbVPrintfW(szBuffer, sizeof(szBuffer), lpszFormat, args);
		//ATLASSERT(SUCCEEDED(hr)); //Output truncated as it was > sizeof(szBuffer)
		if (SUCCEEDED(hr))
		{
			/*StringCbCopyW(szOutText, sizeof(szOutText), _MYDEBUGFLAG_W);
			StringCbCatW(szOutText, sizeof(szOutText), szBuffer);*/
			switch (llLevel)
			{
			case LOG_LEVEL_INFO:
				StringCbPrintfW(szOutText, sizeof(szOutText), FILE_LOG_FORMAT_W, L"INFO  ", ThreadId, lpszCppFileName, dwLine, lpszFunctionName, szBuffer);
				break;

			case LOG_LEVEL_ERROR:
				StringCbPrintfW(szOutText, sizeof(szOutText), FILE_LOG_FORMAT_W, L"ERROR ", ThreadId, lpszCppFileName, dwLine, lpszFunctionName, szBuffer);
				break;

			case LOG_LEVEL_FATAL:
				StringCbPrintfW(szOutText, sizeof(szOutText), FILE_LOG_FORMAT_W, L"FATAL ", ThreadId, lpszCppFileName, dwLine, lpszFunctionName, szBuffer);
				break;

			default:
				StringCbPrintfW(szOutText, sizeof(szOutText), FILE_LOG_FORMAT_W, L"UNKNOW", ThreadId, lpszCppFileName, dwLine, lpszFunctionName, szBuffer);
				break;
			}

			g_SmartLogger.AddLog(szOutText);

			WriteTraceToFile();
		}
	}

	if (g_bEnableDbgView)
	{
		va_list args;
		va_start(args, lpszFormat);

		WCHAR szBuffer[512] = { 0 };
		WCHAR szOutText[512 + 20] = { 0 };

		HRESULT hr = StringCbVPrintfW(szBuffer, sizeof(szBuffer), lpszFormat, args);
		//ATLASSERT(SUCCEEDED(hr)); //Output truncated as it was > sizeof(szBuffer)
		if (SUCCEEDED(hr))
		{
			/*StringCbCopyW(szOutText, sizeof(szOutText), _MYDEBUGFLAG_W);
			StringCbCatW(szOutText, sizeof(szOutText), szBuffer);*/
			switch (llLevel)
			{
			case LOG_LEVEL_INFO:
				StringCbPrintfW(szOutText, sizeof(szOutText), DBGVIEW_LOG_FORMAT_W, L"INFO  ", ThreadId, lpszCppFileName, dwLine, szBuffer);
				break;

			case LOG_LEVEL_ERROR:
				StringCbPrintfW(szOutText, sizeof(szOutText), DBGVIEW_LOG_FORMAT_W, L"ERROR ", ThreadId, lpszCppFileName, dwLine, szBuffer);
				break;

			case LOG_LEVEL_FATAL:
				StringCbPrintfW(szOutText, sizeof(szOutText), DBGVIEW_LOG_FORMAT_W, L"FATAL ", ThreadId, lpszCppFileName, dwLine, szBuffer);
				break;

			default:
				StringCbPrintfW(szOutText, sizeof(szOutText), DBGVIEW_LOG_FORMAT_W, L"UNKNOW", ThreadId, lpszCppFileName, dwLine, szBuffer);
				break;
			}

			OutputDebugStringW(szOutText);

		}
	}

}

VOID _cdecl LogA(BOOL bLog, LOG_LEVEL llLevel, LPCSTR lpszCppFileName, DWORD ThreadId, DWORD dwLine, LPCSTR lpszFunctionName, LPCSTR lpszFormat, ...)
{
	if (bLog)
	{
		va_list args;
		va_start(args, lpszFormat);

		CHAR szBuffer[4096] = { 0 };
		CHAR szOutText[4096 + 20] = { 0 };

		HRESULT hr = StringCbVPrintfA(szBuffer, sizeof(szBuffer), lpszFormat, args);

		if (SUCCEEDED(hr))
		{

			switch (llLevel)
			{
			case LOG_LEVEL_INFO:
				StringCbPrintfA(szOutText, sizeof(szOutText), FILE_LOG_FORMAT_A, "INFO  ", ThreadId, lpszCppFileName, dwLine, lpszFunctionName, szBuffer);
				break;

			case LOG_LEVEL_ERROR:
				StringCbPrintfA(szOutText, sizeof(szOutText), FILE_LOG_FORMAT_A, "ERROR ", ThreadId, lpszCppFileName, dwLine, lpszFunctionName, szBuffer);
				break;

			case LOG_LEVEL_FATAL:
				StringCbPrintfA(szOutText, sizeof(szOutText), FILE_LOG_FORMAT_A, "FATAL ", ThreadId, lpszCppFileName, dwLine, lpszFunctionName, szBuffer);
				break;

			default:
				StringCbPrintfA(szOutText, sizeof(szOutText), FILE_LOG_FORMAT_A, "UNKNOW", ThreadId, lpszCppFileName, dwLine, lpszFunctionName, szBuffer);
				break;
			}


			g_SmartLogger.AddLog(CString(szOutText));

			WriteTraceToFile();
		}
	}

	if (g_bEnableDbgView)
	{
		va_list args;
		va_start(args, lpszFormat);

		CHAR szBuffer[512] = { 0 };
		CHAR szOutText[512 + 20] = { 0 };

		HRESULT hr = StringCbVPrintfA(szBuffer, sizeof(szBuffer), lpszFormat, args);

		if (SUCCEEDED(hr))
		{

			switch (llLevel)
			{
			case LOG_LEVEL_INFO:
				StringCbPrintfA(szOutText, sizeof(szOutText), DBGVIEW_LOG_FORMAT_A, "INFO  ", lpszCppFileName, dwLine, szBuffer);
				break;

			case LOG_LEVEL_ERROR:
				StringCbPrintfA(szOutText, sizeof(szOutText), DBGVIEW_LOG_FORMAT_A, "ERROR ", lpszCppFileName, dwLine, szBuffer);
				break;

			case LOG_LEVEL_FATAL:
				StringCbPrintfA(szOutText, sizeof(szOutText), DBGVIEW_LOG_FORMAT_A, "FATAL ", lpszCppFileName, dwLine, szBuffer);
				break;

			default:
				StringCbPrintfA(szOutText, sizeof(szOutText), DBGVIEW_LOG_FORMAT_A, "UNKNOW", lpszCppFileName, dwLine, szBuffer);
				break;
			}

			////OutputDebugStringA(szOutText);
		}
	}
}

VOID _cdecl WriteTraceToFile()
{
	if (!g_bEnableFileLog && !g_bEnableFileLog1) return;

	g_SmartLogger.SaveToFile();
	g_SmartLogger.Clear();
}




#endif
