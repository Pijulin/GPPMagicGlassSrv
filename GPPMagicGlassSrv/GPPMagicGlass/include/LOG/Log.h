#ifndef ___TRACE_H___
#define ___TRACE_H___

#define LOG_TO_FILE     //鍐欏埌鏂囦欢
#include <windows.h>

//#include "SmartLogger.h"
//
extern BOOL g_bEnableFileLog;
extern BOOL g_bEnableDbgView;
extern BOOL g_bEnableFileLog1;
extern BOOL g_bEnableExtFileLog;

#define _MYDEBUGFLAG_A ""
#define _MYDEBUGFLAG_W L""

typedef enum tagLOG_LEVEL
{
    LOG_LEVEL_UNKNOW,
    LOG_LEVEL_INFO,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_FATAL,
}LOG_LEVEL;

VOID _cdecl WriteTraceToFile();

VOID _cdecl LogA(
    BOOL bLog,
    LOG_LEVEL llLevel,
    LPCSTR lpszCppFileName,
    DWORD ThreadId,
    DWORD dwLine,
    LPCSTR lpszFunctionName,
    LPCSTR lpszFormat,
    ...);

VOID _cdecl LogW(
    BOOL bLog,
    LOG_LEVEL llLevel,
    LPCWSTR lpszCppFileName,
    DWORD ThreadId,
    DWORD dwLine,
    LPCWSTR lpszFunctionName,
    LPCWSTR lpszFormat,
    ...);

VOID _cdecl LogExtW(
    BOOL bLog,
    LOG_LEVEL llLevel,
    LPCWSTR lpszCppFileName,
    DWORD ThreadId,
    DWORD dwLine,
    LPCWSTR lpszFunctionName,
    LPCWSTR lpszFormat,
    ...);

#define ___L(x)      L ## x
#define __L(x)       ___L(x)

#define USE_LOG 1
#ifdef USE_LOG
#define LOGA(level,fmt, ...)     LogA(g_bEnableFileLog,level, __FILE__, GetCurrentThreadId(), __LINE__, __FUNCTION__, fmt, __VA_ARGS__, level)
#define LOGW(level,fmt, ...)     LogW(g_bEnableFileLog,level, __L( __FILE__ ), GetCurrentThreadId(), __LINE__, __L( __FUNCTION__ ), fmt, __VA_ARGS__)

#define LOG1A(level,fmt, ...)     LogA(g_bEnableFileLog1,level, __FILE__, GetCurrentThreadId(), __LINE__, __FUNCTION__, fmt, __VA_ARGS__, level)


//#define LOG LOGA
#if defined(_UNICODE) || defined(UNICODE)
#define LOG LOGW
#define LOGINFO(fmt, ...)     LogW(g_bEnableFileLog,LOG_LEVEL_INFO, __L( __FILE__ ), GetCurrentThreadId(), __LINE__, __L( __FUNCTION__ ), fmt, __VA_ARGS__)
#define LOGERROR(fmt, ...)     LogW(g_bEnableFileLog,LOG_LEVEL_ERROR, __L( __FILE__ ), GetCurrentThreadId(), __LINE__, __L( __FUNCTION__ ), fmt, __VA_ARGS__)
#define LOGFATAL(fmt, ...)     LogW(g_bEnableFileLog,LOG_LEVEL_FATAL, __L( __FILE__ ), GetCurrentThreadId(), __LINE__, __L( __FUNCTION__ ), fmt, __VA_ARGS__)
#define LOGEXT(level, fmt, ...)     LogExtW(g_bEnableExtFileLog,level, __L( __FILE__ ), GetCurrentThreadId(), __LINE__, __L( __FUNCTION__ ), fmt, __VA_ARGS__)
#define LOGEXTINFO(fmt, ...)     LogExtW(g_bEnableExtFileLog,LOG_LEVEL_INFO, __L( __FILE__ ), GetCurrentThreadId(), __LINE__, __L( __FUNCTION__ ), fmt, __VA_ARGS__)
#define LOGEXTERROR(Fmt, ...)     LogExtW(g_bEnableExtFileLog,LOG_LEVEL_ERROR, __L( __FILE__ ), GetCurrentThreadId(), __LINE__, __L( __FUNCTION__ ), fmt, __VA_ARGS__)
#define LOGEXTFATAL(fmt, ...)     LogExtW(g_bEnableExtFileLog,LOG_LEVEL_FATAL, __L( __FILE__ ), GetCurrentThreadId(), __LINE__, __L( __FUNCTION__ ), fmt, __VA_ARGS__)
#define LOGINFOLOG1(fmt, ...)     LogW(g_bEnableFileLog1,LOG_LEVEL_INFO, __L( __FILE__ ), GetCurrentThreadId(), __LINE__, __L( __FUNCTION__ ), fmt, __VA_ARGS__)
#define _L L
#else
#define LOG LOGA
#define LOGINFO(fmt, ...)     LogA(g_bEnableFileLog,LOG_LEVEL_INFO,  __FILE__ , GetCurrentThreadId(), __LINE__,  __FUNCTION__ , fmt, __VA_ARGS__)
#define LOGERROR(fmt, ...)     LogA(g_bEnableFileLog,LOG_LEVEL_ERROR,  __FILE__ , GetCurrentThreadId(), __LINE__,  __FUNCTION__ , fmt, __VA_ARGS__)
#define LOGFATAL(fmt, ...)     LogA(g_bEnableFileLog,LOG_LEVEL_FATAL,  __FILE__ , GetCurrentThreadId(), __LINE__,  __FUNCTION__ , fmt, __VA_ARGS__)
#define LOGINFOW(fmt, ...)     LogW(g_bEnableFileLog, LOG_LEVEL_INFO, __L(__FILE__), GetCurrentThreadId(),  __LINE__, __L(__FUNCTION__), fmt, __VA_ARGS__)
#define _L 
#endif

#else 
#define LOG(...) 
#define LOGA(...) 
#define LOGW(...) 
#define LOGINFO(fmt, ...) 
#define LOGERROR(fmt, ...) 
#define LOGFATAL(fmt, ...)
#define LOGINFOW(fmt, ...) 
#define _L L
#endif //End LOG_ON

#endif //End ___TRACE_H___
