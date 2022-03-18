#pragma once
#include <windows.h>
#include <tchar.h>
#include <TlHelp32.h>
#include <Shlobj.h>
#include <WinIoCtl.h>

namespace XProcess {
	static BOOL IsProcessExists(LPCTSTR lpProcName)
	{
		if (lpProcName == NULL)
			return FALSE;

		PROCESSENTRY32 pe32;
		pe32.dwSize = sizeof(PROCESSENTRY32);

		HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (hProcessSnap == INVALID_HANDLE_VALUE)
			return FALSE;

		BOOL bMore = ::Process32First(hProcessSnap, &pe32);
		while (bMore)
		{
			if (_tcsicmp(lpProcName, pe32.szExeFile) == 0)
			{
				CloseHandle(hProcessSnap);
				return TRUE;
			}

			bMore = ::Process32Next(hProcessSnap, &pe32);
		}

		CloseHandle(hProcessSnap);

		return FALSE;
	}
	static BOOL IsProcessExists(DWORD dwPid)
	{
		PROCESSENTRY32 pe32;
		pe32.dwSize = sizeof(PROCESSENTRY32);

		HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (hProcessSnap == INVALID_HANDLE_VALUE)
			return FALSE;

		BOOL bMore = ::Process32First(hProcessSnap, &pe32);
		while (bMore)
		{
			if (dwPid == pe32.th32ProcessID)
			{
				CloseHandle(hProcessSnap);
				return TRUE;
			}

			bMore = ::Process32Next(hProcessSnap, &pe32);
		}

		CloseHandle(hProcessSnap);

		return FALSE;
	}
	static BOOL IsSpecificWindowExists(LPCTSTR lpWindowClass, LPCTSTR lpWindow)
	{
		HWND hWnd = NULL;
		hWnd = ::FindWindow(lpWindowClass, lpWindow);
		if (hWnd)
			return TRUE;

		return FALSE;
	}

	static HANDLE GetProcessHandleByName(LPCTSTR lpProcName)
	{
		HANDLE hProcess = NULL;
		if (lpProcName == NULL)
			return hProcess;

		PROCESSENTRY32 pe32;
		pe32.dwSize = sizeof(PROCESSENTRY32);

		HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (hProcessSnap == INVALID_HANDLE_VALUE)
			return hProcess;

		BOOL bMore = ::Process32First(hProcessSnap, &pe32);
		while (bMore)
		{
			if (_tcsicmp(lpProcName, pe32.szExeFile) == 0)
			{
				hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
				break;
			}

			bMore = ::Process32Next(hProcessSnap, &pe32);
		}

		CloseHandle(hProcessSnap);

		return hProcess;
	}

}

namespace  XUtilis {

	static BOOL IsUSBRemovableDrive(const CString& strDrive)
	{
		if (strDrive.Find(_T(":\\")) != 1 && strDrive.Find(_T(":/")) != 1)  //格式不正确
		{
			return FALSE;
		}

		CString strFmtDrive = strDrive.Left(strDrive.Find(_T(":")) + 1);

		CString strFile = _T("\\\\.\\") + strFmtDrive;
		HANDLE  pchFile = NULL;
		DWORD	dwLen = 0;
		char    pcbBuf[2 * 1024];
		PSTORAGE_DEVICE_DESCRIPTOR pchDevInfo = (PSTORAGE_DEVICE_DESCRIPTOR)pcbBuf;
		STORAGE_PROPERTY_QUERY  hQueryInfo;
		if ((pchFile = CreateFile(strFile, STANDARD_RIGHTS_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)) == INVALID_HANDLE_VALUE)
		{
			return FALSE;
		}

		hQueryInfo.PropertyId = StorageDeviceProperty;
		hQueryInfo.QueryType = PropertyStandardQuery;
		pchDevInfo->Size = sizeof(pcbBuf);
		if (DeviceIoControl(pchFile, IOCTL_STORAGE_QUERY_PROPERTY, &hQueryInfo, sizeof(STORAGE_PROPERTY_QUERY), pchDevInfo, pchDevInfo->Size, &dwLen, NULL) == FALSE)
		{
			CloseHandle(pchFile);
			return FALSE;
		}

		BOOL bRet = FALSE;
		switch (pchDevInfo->BusType)
		{
		case BusType1394:
		case BusTypeUsb:
			bRet = TRUE;
		default:
			break;
		}

		CloseHandle(pchFile);
		return bRet;
	}
	static BOOL IsFixedDisk(CString strPath)
	{
		return (GetDriveType(strPath.GetBuffer()) == DRIVE_FIXED && !IsUSBRemovableDrive(strPath));
	}
	static BOOL GetMaxFreeSpaceFixedDisk_NonSysDisk(CString& strDrive, CString& strFreeSpace)
	{
		CString sSysDir;
		if (SHGetSpecialFolderPath(NULL, sSysDir.GetBuffer(MAX_PATH), CSIDL_PROGRAM_FILES, FALSE))
		{
			sSysDir.ReleaseBuffer();
		}

		TCHAR szDriverBuffer[512] = { 0 };
		CString sDriver, sResult;
		DWORD dwLength = GetLogicalDriveStrings(512, szDriverBuffer);

		ULARGE_INTEGER uli;
		ULONGLONG ulilast = 0;
		TCHAR *lpDriverBuffer = szDriverBuffer;
		while (*lpDriverBuffer != NULL)
		{
			sDriver = lpDriverBuffer;
			if (sSysDir.Find(sDriver.GetBuffer()) == -1 && IsFixedDisk(sDriver.GetBuffer()) && GetDiskFreeSpaceEx(sDriver.GetBuffer(), &uli, NULL, NULL))
			{
				if (uli.QuadPart >= ulilast)
				{
					ulilast = uli.QuadPart;
					sResult = sDriver;
				}
			}
			lpDriverBuffer = lpDriverBuffer + sDriver.GetLength() + 1;
		}

		strDrive = sResult;

		return TRUE;
	}

}
