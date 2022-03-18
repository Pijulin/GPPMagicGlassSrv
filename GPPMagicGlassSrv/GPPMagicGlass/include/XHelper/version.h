#pragma once
#include <atlstr.h>
//#include <winNt.h>

namespace  Version {

	__inline CString GetWindowsVersion()
	{
		CString szVersion;

		/*typedef NTSTATUS(NTAPI* fnRtlGetVersion)(PRTL_OSVERSIONINFOW lpVersionInformation);
		RTL_OSVERSIONINFOEXW verInfo = { 0 };
		verInfo.dwOSVersionInfoSize = sizeof(verInfo);

		static auto RtlGetVersion = (fnRtlGetVersion)GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "RtlGetVersion");

		if (RtlGetVersion != 0 && RtlGetVersion((PRTL_OSVERSIONINFOW)&verInfo) == 0)
		{
			szVersion.Format(_T("%d.%d.%d"), verInfo.dwMajorVersion, verInfo.dwMinorVersion, verInfo.dwBuildNumber);
			return szVersion;
		}
		else*/
		{
			OSVERSIONINFO ovi = { sizeof(OSVERSIONINFO) };
			BOOL bRet = ::GetVersionEx(&ovi);
			if (bRet)
			{
				szVersion.Format(_T("%d.%d.%d"), ovi.dwMajorVersion, ovi.dwMinorVersion, ovi.dwBuildNumber);
				return szVersion;
			}
		}
		return _T("");
	}

	__inline CString GetFullPath()
	{
		TCHAR szPath[MAX_PATH * 2] = {};
		GetModuleFileName(NULL, szPath, _countof(szPath));

		CString szProcessName = szPath;
		return szProcessName;
	}

	__inline BOOL GetFileVersion(LPCTSTR strFile, CString& strVersion)
	{
		DWORD dwVerSize;
		DWORD dwHandle;
		void* bufVer;
		BOOL bReturn = FALSE;

		strVersion.Empty();

		//	SetErrorMode(SEM_FAILCRITICALERRORS);

		dwVerSize = GetFileVersionInfoSize(strFile, &dwHandle);
		if (dwVerSize == 0)
			return FALSE;

		bufVer = malloc(dwVerSize);
		if (!bufVer)
			return FALSE;

		if (GetFileVersionInfo(strFile, 0, dwVerSize, bufVer))
		{
			VS_FIXEDFILEINFO* pInfo;
			unsigned int nInfoLen;

			bReturn = VerQueryValue(bufVer, _T("\\"), (void**)&pInfo, &nInfoLen);

			strVersion.Format(
				_T("%d.%d.%d.%d"),
				HIWORD(pInfo->dwFileVersionMS), LOWORD(pInfo->dwFileVersionMS),
				HIWORD(pInfo->dwFileVersionLS), LOWORD(pInfo->dwFileVersionLS));
		}
		free(bufVer);

		return bReturn;
	}
	__inline BOOL GetForeFileVersion(LPCTSTR strFile, CString& strVersion)
	{
		DWORD dwVerSize;
		DWORD dwHandle;
		void* bufVer;
		BOOL bReturn = FALSE;

		strVersion.Empty();

		//	SetErrorMode(SEM_FAILCRITICALERRORS);

		dwVerSize = GetFileVersionInfoSize(strFile, &dwHandle);
		if (dwVerSize == 0)
			return FALSE;

		bufVer = malloc(dwVerSize);
		if (!bufVer)
			return FALSE;

		if (GetFileVersionInfo(strFile, 0, dwVerSize, bufVer))
		{
			VS_FIXEDFILEINFO* pInfo;
			unsigned int nInfoLen;

			bReturn = VerQueryValue(bufVer, _T("\\"), (void**)&pInfo, &nInfoLen);

			strVersion.Format(
				_T("%d.%d"),
				HIWORD(pInfo->dwFileVersionMS), LOWORD(pInfo->dwFileVersionMS));
		}
		free(bufVer);

		return bReturn;
	}
	__inline CString GetAppVersion()
	{
		CString szFilePath = GetFullPath();

		CString szFileVersion = _T("");
		if (GetFileVersion(szFilePath, szFileVersion))
		{
			return szFileVersion;
		}
		else
			return _T("");
	}
}