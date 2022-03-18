#pragma once
namespace XFileAttr
{
	struct LANGANDCODEPAGE {
		WORD wLanguage;
		WORD wCodePage;
	}*lpTranslate;

	static CString GetFileInfoEx(LPCTSTR szFilePath, LPCTSTR szSectionName)
	{
		CString szInfo = _T("");

		INT nVersionLen = GetFileVersionInfoSize(szFilePath, NULL);
		if (nVersionLen > 0)
		{
			BYTE* pBuffer = new BYTE[nVersionLen];
			if (GetFileVersionInfo(szFilePath, NULL, nVersionLen, pBuffer))
			{
				UINT uBytes = 0;
				UINT nTCHARCount = 0;
				LPCWSTR lpStr = NULL;
				WORD* langinfo = NULL;
				if (VerQueryValue(pBuffer, _T("\\VarFileInfo\\Translation"), (LPVOID*)&langinfo, &uBytes))
				{
					CString strSubBlock;
					strSubBlock.Format(_T("\\StringFileInfo\\%04x%04x\\%s"), langinfo[0], langinfo[1], szSectionName);
					if (VerQueryValue(pBuffer, strSubBlock, (LPVOID*)&lpStr, &nTCHARCount))
					{
						szInfo = CString(lpStr, nTCHARCount);
					}
				}
			}
			delete[] pBuffer;
		}

		return szInfo;
	}
	static CString GetFileVersion(LPCTSTR szFilePath)
	{
		DWORD dwVerSize;
		DWORD dwHandle;
		void* bufVer;

		CString szInfo = _T("");

		dwVerSize = GetFileVersionInfoSize(szFilePath, &dwHandle);
		if (dwVerSize>0)
		{
			bufVer = malloc(dwVerSize);
			if (!bufVer)
				return szInfo;

			if (GetFileVersionInfo(szFilePath, 0, dwVerSize, bufVer))
			{
				VS_FIXEDFILEINFO* pInfo;
				unsigned int nInfoLen;

				if (VerQueryValue(bufVer, _T("\\"), (void**)&pInfo, &nInfoLen))
				{
					szInfo.Format(
						_T("%d.%d.%d.%d"),
						HIWORD(pInfo->dwFileVersionMS), LOWORD(pInfo->dwFileVersionMS),
						HIWORD(pInfo->dwFileVersionLS), LOWORD(pInfo->dwFileVersionLS));
				}
			}
			free(bufVer);
		}



		return szInfo;
	}
};

