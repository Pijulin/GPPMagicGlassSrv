
#include "AutoLocker.h"
#include "SmartLogger.h"
#include "Log.h"

#include <locale>
#include <shlobj.h>
#include <strsafe.h>
#include <psapi.h>



#define LOG_PREFIX_NAME	TEXT("")
#define LOG_PREFIX_LEN	_countof(LOG_PREFIX_NAME)

static const int const_log_size_limit = 1024 * 1024 * 1024; //1G 


static HMODULE GetCurrentModuleHandle(BOOL IsUnchangedRefcount = TRUE)
{
	DWORD flags = GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS;

	if (IsUnchangedRefcount)
		flags |= GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT;

	HMODULE hSelf = NULL;
	GetModuleHandleEx(
		flags,
		(LPCTSTR)&GetCurrentModuleHandle,
		&hSelf);

	return hSelf;
}

CSmartLogger::CSmartLogger()
	: m_bFirstWrite(TRUE)
{
	TCHAR processbuffer[MAX_PATH] = { 0 };
	size_t length = 0;
	//int i = 0,j = 0;
	GetModuleFileName(GetModuleHandle(NULL), processbuffer, MAX_PATH);
	length = _tcslen(processbuffer);
	for (UINT i = (UINT)length - 1; i >= 0; i--)//从路径后面开始找\，即倒着找右斜杠  
	{
		if (processbuffer[i] == '\\')//找到第一个\，就可以马上获取进程名称了
		{
			i++;
			for (UINT j = 0; i <= length; j++)//结束符\0不能少,即i=length  
			{
				processbuffer[j] = processbuffer[i++];
			}
			break;
		}
	}
	setlocale(LC_ALL, "chs");

	CString szLogFilePath;
	TCHAR szPath[MAX_PATH] = {};

	GetEnvironmentVariable(TEXT("APPDATA"), szPath, MAX_PATH);
	::PathRemoveFileSpec(szPath);

	szLogFilePath.Format(TEXT("%s\\Local\\GamePP\\app.log.on"), szPath);
	g_bEnableFileLog = PathFileExists(szLogFilePath);

	szLogFilePath.Format(TEXT("%s\\Local\\GamePP\\app.log1.on"), szPath);
	g_bEnableFileLog1 = PathFileExists(szLogFilePath);

	szLogFilePath.Format(TEXT("%s\\Local\\GamePP\\app.ext.log.on"), szPath);
	g_bEnableExtFileLog = PathFileExists(szLogFilePath);

	szLogFilePath.Format(TEXT("%s\\Local\\GamePP\\app.dbg.on"), szPath);
	g_bEnableDbgView = PathFileExists(szLogFilePath);

	m_strFilePath.Format(TEXT("%s\\Local\\GamePP\\Logs\\%s_%d.log"),
		szPath, processbuffer, GetCurrentProcessId());
}

CSmartLogger::~CSmartLogger()
{
	Clear();
}

int CSmartLogger::AddLog(const CString& szLine)
{
	_BaseModule::CAutoLocker __locker(&m_cs);

	CString szText = szLine;
	if (szText.GetLength() > 0 && szText.GetAt(szText.GetLength() - 1) != '\n')
	{
		szText += _T("\n");
	}

	SYSTEMTIME   st = { 0 };
	GetLocalTime(&st);

	TCHAR szTime[100] = { 0 };
	StringCbPrintf(szTime, 1024, TEXT("%04d-%02d-%02d %02d:%02d:%02d "),
		st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

	m_container.push_back(szTime + szText);
	if (m_container.size() > 100)
	{
		SaveToFile();
	}

	return static_cast<int>(m_container.size());
}

int CSmartLogger::AddLog(LPCTSTR lpszFormat, va_list argList)
{
	_BaseModule::CAutoLocker __locker(&m_cs);

	if (!lpszFormat) return static_cast<int>(m_container.size());

	SYSTEMTIME   st = { 0 };
	GetLocalTime(&st);

	TCHAR szTime[100] = { 0 };
	StringCbPrintf(szTime, 1024, TEXT("%04d-%02d-%02d %02d:%02d:%02d "),
		st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

	const int nBufSize = 2048;
	TCHAR szCommonBuffer_Full[nBufSize + 200] = LOG_PREFIX_NAME;
	TCHAR* szCommonBuffer = szCommonBuffer_Full + LOG_PREFIX_LEN;

	StringCbVPrintf(szCommonBuffer, 1024 - LOG_PREFIX_LEN, lpszFormat, argList);
	int len = (int)_tcslen(szCommonBuffer) - 1;
	if (szCommonBuffer[len] != TEXT('\n'))
	{
		szCommonBuffer[len + 1] = TEXT('\n');
		szCommonBuffer[len + 2] = TEXT('\0');
	}

	m_container.push_back(CString(szTime) + szCommonBuffer_Full);

	return static_cast<int>(m_container.size());
}

VOID CSmartLogger::Clear(VOID)
{
	_BaseModule::CAutoLocker __locker(&m_cs);

	m_container.clear();
}

BOOL CSmartLogger::SaveToFile(VOID)
{
	_BaseModule::CAutoLocker __locker(&m_cs);

	BOOL bResult = FALSE;

	int nFileNameLen = (int)_tcslen(m_strFilePath) + 1;
	LPTSTR lpszPath = new TCHAR[nFileNameLen];
	memset(lpszPath, 0, nFileNameLen * sizeof(TCHAR));
	StringCbCopy(lpszPath, sizeof(TCHAR) * (nFileNameLen), m_strFilePath);
	PathRemoveFileSpec(lpszPath);

	if (!PathFileExists(lpszPath))
	{
		//::SHCreateDirectoryEx(NULL, lpszPath, NULL);
		CreateDirectory(lpszPath, NULL);
	}

	if (PathFileExists(lpszPath))
	{
		//MessageBox(NULL,L"Exist",NULL,MB_OK);
		SYSTEMTIME   st = { 0 };
		GetLocalTime(&st);

		TCHAR szTime[100] = { 0 };
		StringCbPrintf(szTime, 1024, TEXT("%04d-%02d-%02d %02d:%02d:%02d "),
			st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

		DWORD dwFileAttributes = 0;

		dwFileAttributes = GetFileAttributes(m_strFilePath);

		FILE* pLogFile = NULL;
		_tfopen_s(&pLogFile, m_strFilePath, _T("a"));
		if (pLogFile)
		{
			bResult = TRUE;

			if (m_bFirstWrite)
			{
				if (INVALID_FILE_ATTRIBUTES == dwFileAttributes)
				{
					TCHAR szProcessPath[MAX_PATH] = {};
					GetModuleFileName(GetModuleHandle(NULL), szProcessPath, sizeof(szProcessPath));

					_ftprintf(pLogFile, TEXT("\r\n================== Start Log:%s ==================\n")
						TEXT("Process Information\n")
						TEXT("PID: %d\n")
						TEXT("EXE: %s\n")
						TEXT("================================================================================================================================================\n\n")
						, szTime, GetCurrentProcessId(), szProcessPath);
				}
				else
				{
					_ftprintf(pLogFile, TEXT("\n"));
				}

				m_bFirstWrite = FALSE;
			}

			int nTotalCnt = (int)m_container.size();
			for (int i = 0; i < nTotalCnt; i++)
			{
				_ftprintf(pLogFile, m_container[i]);
			}

			fflush(pLogFile);
			fclose(pLogFile);
		}
	}

	if (lpszPath)
	{
		delete[] lpszPath;
	}
	Clear();

	return bResult;
}

VOID CSmartLogger::SetLogFileName(const CString& strLogFile)
{
	// 	TCHAR szLogFile[MAX_PATH] = { 0 };
	// 	SHGetSpecialFolderPath(NULL, szLogFile, CSIDL_APPDATA, FALSE);
	// 	PathAppend( szLogFile, DIR_360SAFE );
	// 	PathAppend( szLogFile, strLogFile );
	m_strFilePath = strLogFile;

	if (PathFileExists(m_strFilePath))
	{
		LARGE_INTEGER lgSize = GetFileSize(m_strFilePath);
		if (lgSize.QuadPart > const_log_size_limit)
		{
			::DeleteFile(m_strFilePath);
		}
	}
}

LARGE_INTEGER CSmartLogger::GetFileSize(CString& strFile)
{
	LARGE_INTEGER llFileSize;
	llFileSize.QuadPart = 0;

	if (strFile.IsEmpty())
		return llFileSize;

	HANDLE hFile;
	hFile = CreateFile(strFile, GENERIC_READ | GENERIC_WRITE, 0, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
		return llFileSize;


	::GetFileSizeEx(hFile, &llFileSize);

	::CloseHandle(hFile);

	return llFileSize;
}
