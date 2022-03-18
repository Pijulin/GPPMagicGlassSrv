#pragma once
#include <Statistics\Statistics.h>
#include <MAC\MID.h>
#include <MD5\Md5.h>
#include "reportcode.h"
#include "version.h"
#include <atlutil.h>
#include <WinHTTPLib\WinHTTPLib.h>
#include <unicodeutils\unicodeconv.h>
#include <FCFileEx.h>
#include <json\json.h>
#include <vector>
#include <time.h>

#pragma comment(lib,"json_vc71_libmd_vs10.lib")

#define UPLOAD_URL _T("http://cj.gamepp.com/cj?mac={mac}&action={action}&os={os}")
#define UPLOAD_OSS_URL _T("http://cj.gamepp.com/cj?mac={mac}&action={action}&os={os}&errormsg={errormsg}")

#define UPLOAD_IMAGE_URL   _T("http://pubg.gamepp.com/uproominfo.php?roomid={roomid}&username={username}")
#define DOWNLOAD_IMAGE_URL _T("http://pubg.gamepp.com/groominfo.php?roomid={roomid}")
#define CHECK_IMAGE_URL    _T("http://pubg.gamepp.com/groomtm.php?roomid={roomid}")

#define PUSH_MESSAGE_URL    _T("http://ptip.gamepp.com/ptip.php?v={v}")
#define OSS_INFORMATION_URL _T("http://liveupdate.gamepp.com/oss.php?version={version}")


//
#define WINDOW_INFORMATION_URL _T("http://liveupdate.gamepp.com/cef/window.php?version={version}&window={window}&type={type}")

#define ROOM_ID   _T("{roomid}")
#define USERNAME  _T("{username}")

#define URL_MAC	        _T("{mac}")
#define URL_ACTION		_T("{action}")
#define URL_OS			_T("{os}")

typedef struct _ROOM_IMAGE {
	TCHAR ImagePath[MAX_PATH];
	TCHAR UserName[100];
	DWORD TimeStamp;
}ROOM_IMAGE;


typedef struct _PUSH_MESSAGE {
	UINT uXPos;
	UINT uYPos;
	UINT uMsgID;
	UINT uCloseState;
}PUSH_MESSAGE;


typedef std::vector<ROOM_IMAGE>  VEC_IMAGE;

class  UPLoad {

public:
	static void SendStatics(_REPORT_STATE  nState, BOOL bFlag)  
	{		
		CString szUrl = UPLOAD_URL;

		CString szMacID = GetMacIDEx();
		CString szAction;
		szAction.Format(_T("%d"), nState);
		CString szOS      = Version::GetWindowsVersion();
		CString szVersion = Version::GetAppVersion();
		//CString szChannel = Utils::GetChannelForUpdate();

		if (szUrl.Find(URL_MAC) > 0)
		{
			szUrl.Replace(URL_MAC, szMacID);
		}

		if (szUrl.Find(URL_ACTION) > 0)
		{
			szUrl.Replace(URL_ACTION, szAction);
		}

		if (szUrl.Find(URL_OS) > 0)
		{
			szUrl.Replace(URL_OS, szOS);
		}

		CStatistic::GetInstance()->SendStatUrl(szUrl, bFlag);
	}

	static void SendOSSError(_REPORT_STATE  nState, BOOL bFlag,std::string strError)
	{
		CString szUrl = UPLOAD_OSS_URL;

		CString szMacID = GetMacIDEx();
		CString szAction;
		szAction.Format(_T("%d"), nState);
		CString szOS = Version::GetWindowsVersion();
		CString szVersion = Version::GetAppVersion();
		CString ErrorMsg;
		ErrorMsg = URLEncode(strError.c_str());

		if (szUrl.Find(URL_MAC) > 0)
		{
			szUrl.Replace(URL_MAC, szMacID);
		}

		if (szUrl.Find(URL_ACTION) > 0)
		{
			szUrl.Replace(URL_ACTION, szAction);
		}

		if (szUrl.Find(URL_OS) > 0)
		{
			szUrl.Replace(URL_OS, szOS);
		}

		if (szUrl.Find(_T("{errormsg}")) > 0)
		{
			szUrl.Replace(_T("{errormsg}"), ErrorMsg);
		}



		CStatistic::GetInstance()->SendStatUrl(szUrl, bFlag);
	}


	static CString URLEncode(LPCSTR szURL)
	{
		CString szRet;
		const DWORD dwBufferLen = 2048;
		DWORD dwLen = 0;
		CHAR szBuffer[dwBufferLen] = {};
		if (AtlEscapeUrl(szURL, szBuffer, &dwLen, dwBufferLen))
		{
			szRet = szBuffer;
		}
		else
		{
			szRet = _T("");
		}
		return szRet;
	}

	static BOOL UploadImage(LPCTSTR  lpUrl, LPCTSTR szImgPath)
	{
	
		CWinHTTPLib http;

		//
		CString szBoundary = http.CreateUploadBoundary();
		http.SetBoundary(szBoundary);

		CString szDes;

		std::string szPostData;

		//?′?óé?±¨?úèY
		szPostData.append("--");
		szPostData.append(unicodeutil::ANSIFromUnicode(szBoundary));
		
		CString szPicPath = szImgPath;
		CString szPicName = szPicPath.Right(szPicPath.GetLength() - szPicPath.ReverseFind('\\') - 1);
		szPicName = URLEncode(CT2A(szPicName, CP_UTF8));

		szDes.Format(_T("\r\nContent-Disposition: form-data; name=\"file\"; filename=\"%s\"\r\nContent-Type: image/png\r\n\r\n"), szPicName.GetBuffer());
		szPostData.append(unicodeutil::ANSIFromUnicode(szDes));

		std::vector<BYTE> pbuf;
		FCFileEx::Read(szImgPath, pbuf);
		int iSize = pbuf.size();
		szPostData.append((char*)&pbuf[0], pbuf.size());
		szPostData.append("\r\n");

		//?áê?
		szPostData.append("\r\n--");
		szPostData.append(unicodeutil::ANSIFromUnicode(szBoundary));
		szPostData.append("--");

		//·￠?íêy?Y
		http.SendData(lpUrl, szPostData);

		std::string szHTMLContent = http.GetBodyText();

		return TRUE;
	}

	//PreTimestamp: é?′??üD?μ?ê±??′á
	static BOOL CheckRefresh(LPCTSTR  lpUrl, DWORD PreTimestamp)
	{
		CWinHTTPLib http;
		//Get·?ê???è?êy?Y￡¨??è?ê±??′á￡?
		http.SendData(lpUrl, _T(""));
		std::string szTimeStampContent = http.GetBodyText();
		int iTimestamp = atoi(szTimeStampContent.c_str());

		//??óD?üD?
		if (iTimestamp <= (int)PreTimestamp)
			return FALSE;

		return TRUE;
	}
	static BOOL DownloadImage(LPCTSTR  lpUrl, DWORD& PreTimestamp,VEC_IMAGE&  vecIamge)
	{
		BOOL bResult = FALSE;

		TCHAR lpFile[MAX_PATH] = {};
		::GetModuleFileName(NULL,lpFile,MAX_PATH);
		::PathRemoveFileSpec(lpFile);
		_tcscat(lpFile,_T("\\vision\\"));

		CWinHTTPLib http;
		

		//Get·?ê???è?êy?Y
		http.SendData(lpUrl, _T(""));
		std::string szHTMLContent = http.GetBodyText();

		//?a??jsonêy?Y
		Json::Value root;
		Json::Reader reader;
		if (reader.parse(szHTMLContent, root))
		{
			PreTimestamp = root["timestamp"].asInt();

			int iCount = root["data"].size();
			for (int i = 0; i < iCount; i++)
			{
				ROOM_IMAGE  Element;
				CString szFile = root["data"][i]["pic"].asCString();
				CString szUserName = root["data"][i]["username"].asCString();
				CString szDownFile = lpFile;
				szDownFile.Append(szFile.Right(szFile.GetLength() - szFile.ReverseFind('/') - 1));

				_tcscpy(Element.UserName, szUserName.GetBuffer());
				szUserName.ReleaseBuffer();
				//Element.TimeStamp = root[i]["timestamp"].asInt();

				//???????t
				if (S_OK == URLDownloadToFile(NULL, szFile, szDownFile, 0, NULL))
				{
					_tcscpy(Element.ImagePath, szDownFile.GetBuffer());
					szDownFile.ReleaseBuffer();

					vecIamge.push_back(Element);
					bResult = TRUE;
				}
			}
		}

		return bResult;
	}

	static	BOOL GetFileVersion(LPCTSTR strFile, CString& strVersion)
	{
		DWORD dwVerSize;
		DWORD dwHandle;
		void* bufVer;
		BOOL bReturn=FALSE;

		strVersion.Empty();

		//	SetErrorMode(SEM_FAILCRITICALERRORS);

		dwVerSize=GetFileVersionInfoSize(strFile,&dwHandle);
		if(dwVerSize == 0)
			return FALSE;

		bufVer = malloc(dwVerSize);
		if(!bufVer)
			return FALSE;

		if(GetFileVersionInfo(strFile,0,dwVerSize,bufVer))
		{
			VS_FIXEDFILEINFO* pInfo;
			unsigned int nInfoLen;

			bReturn=VerQueryValue(bufVer, _T("\\"), (void**)&pInfo, &nInfoLen);

			strVersion.Format(
				_T("%d.%d.%d.%d"),
				HIWORD( pInfo->dwFileVersionMS ), LOWORD( pInfo->dwFileVersionMS ),
				HIWORD( pInfo->dwFileVersionLS ), LOWORD( pInfo->dwFileVersionLS ) );
		}
		free(bufVer);

		return bReturn;
	}

	//????????????
	static CString GetAppVersion(LPCTSTR lpFilePath)
	{
		CString szFileVersion = _T("");
		if (GetFileVersion(lpFilePath,szFileVersion))
		{
			return szFileVersion;
		}
		else
		    return _T("");
	}
	static std::string ReadInformation(LPCTSTR  lpUrl)
	{
		CWinHTTPLib http;
		http.SendData(lpUrl, _T(""));

		std::string szHTMLContent = http.GetBodyText();


		return szHTMLContent;
	}

	static std::string ReadOSSInformation(LPCTSTR  lpUrl,LPCTSTR lpVersionFile)
	{
		CString szURL = lpUrl;

		CString szVersion = GetAppVersion(lpVersionFile);
		if (szURL.Find(_T("{version}")) != -1)
		{
			szURL.Replace(_T("{version}"), szVersion);
		}

		return ReadInformation(szURL);
	}

	static std::string ReadWindowInformation(LPCTSTR  lpUrl,LPCTSTR lpWindow,LPCTSTR lpType,LPCTSTR lpVersionFile)
	{
		CString szURL = lpUrl;

		if (szURL.Find(_T("{window}")) != -1)
		{
			szURL.Replace(_T("{window}"), lpWindow);
		}

		if (szURL.Find(_T("{type}")) != -1)
		{
			szURL.Replace(_T("{type}"), lpType);
		}

		CString szVersion = GetAppVersion(lpVersionFile);
		if (szURL.Find(_T("{version}")) != -1)
		{
			szURL.Replace(_T("{version}"), szVersion);
		}	


		return ReadInformation(szURL);
	}

	
	
	static BOOL DownloadImage(LPSTR  lpUrl, LPSTR DownFile)
	{
		BOOL bResult = FALSE;

		if (S_OK == URLDownloadToFileA(NULL, lpUrl, DownFile, 0, NULL))
		{
			bResult = TRUE;
		}

		return bResult;
	}

	static CString GetCityFromIP(INT ip)
	{
		CString szUrl;
		CString szInfo = TEXT("");

		try {
			char* sIP = inet_ntoa(*(struct in_addr*)&ip);
			
			CString szIP(sIP);
			
			CWinHTTPLib http;
			http.SetAccept(L"application/json");
			http.SetContentType(L"application/x-www-form-urlencoded");
			http.SetAcceptEncoding(L"gzip, deflate");

			CStringA szPostData;
			szPostData.Format("ip=%s", sIP);


			//szUrl.Format(TEXT("http://int.dpool.sina.com.cn/iplookup/iplookup.php?format=json&ip=%s"), szIP.GetBuffer());
			//szIP.ReleaseBuffer();

			if (http.SendData(_T("http://ip.taobao.com/service/getIpInfo2.php"), szPostData))
			{
				
				std::string szHTMLContent = http.GetBodyText();
				CString strUTF8 = unicodeutil::UTF8Decode(szHTMLContent.c_str());
				std::string szBody = unicodeutil::ANSIFromUnicode(strUTF8);

				
				if (szBody.empty()
					|| szBody.find("country") == std::string::npos
					|| szBody.find("region") == std::string::npos
					|| szBody.find("city") == std::string::npos
					|| szBody.find("isp") == std::string::npos
					)
				{
				
					//LOGINFO(_T("GetCityFromIP 1"));
					return szInfo;
				}
				
				Json::Value  valueData;
				Json::Reader reader;
				bool bRet = reader.parse(szBody, valueData);
				if (!bRet)
				{
					//LOGINFO(_T("GetCityFromIP 2"));
					return szInfo;
				}

				std::string szCountry = "";
				std::string szProvince = "";
				std::string szCity = "";
				std::string szISP = "";
				std::string szDistrict = "";


				szCountry = valueData["data"]["country"].asString();
				szProvince = valueData["data"]["region"].asString();
				szCity = valueData["data"]["city"].asString();
				szISP = valueData["data"]["isp"].asString();


			
				CString szCountry2 = CA2T(szCountry.c_str());
				CString szProvince2 = CA2T(szProvince.c_str());
				CString szCity2 = CA2T(szCity.c_str());
				CString szISP2 = CA2T(szISP.c_str());
				//CString szDistrict2 = CA2T(szDistrict.c_str());

				CString szChina = _T("中国");
				CString szChongqin = _T("重庆");
				CString szShanghai = _T("上海");
				CString szTianjing = _T("天津");
				CString szBeijing = _T("北京");

				szCity2 = szCity2.MakeLower();
				if (szCity2.Find(L"xx") != -1)
				{
					szCity2 = L"";
				}

				if (szChina.CompareNoCase(szCountry2) == 0)
				{
					if (szChongqin.CompareNoCase(szProvince2) == 0
						|| szShanghai.CompareNoCase(szProvince2) == 0
						|| szTianjing.CompareNoCase(szProvince2) == 0
						|| szBeijing.CompareNoCase(szProvince2) == 0)
						szInfo.Format(_T("%s%s"), (LPCTSTR)szCity2, (LPCTSTR)szISP2);
					else
						szInfo.Format(_T("%s%s%s"), (LPCTSTR)szProvince2, (LPCTSTR)szCity2, (LPCTSTR)szISP2);
				}
				else
				{
					szInfo.Format(_T("%s%s"), (LPCTSTR)szCountry2, (LPCTSTR)szCity2);
				}

			

			}
		}
		catch (...)
		{

		}
		//LOGINFO(_T("GetCityFromIP 3"));
		return szInfo;
	}

	static bool IsExist(std::string szSource,LPSTR lpStr)
	{
		if (szSource.find(lpStr) != -1)
			return true;
		return false;
	}
	static BOOL HttpDownloadFile(LPTSTR  lpUrl, LPTSTR DownFile)
	{
		BOOL bResult = FALSE;
		CWinHTTPLib http;

		if( http.SendData(lpUrl, _T(""))
			&& http.SaveToFile(DownFile))
		{
			bResult = TRUE;;
		}
	
		return bResult;
	}
	static void ParsePushMessag(LPCTSTR lpVersion,std::string& szMsg, PUSH_MESSAGE& pMsg)
	{
		CWinHTTPLib http;

		CString szURL = PUSH_MESSAGE_URL;
		std::string stMsg = "";
		
		if (szURL.Find(_T("{v}")))
		{
			szURL.Replace(_T("{v}"),lpVersion);
		}

		http.SendData(szURL, _T(""));
		std::string szHTMLContent = http.GetBodyText();

		//??
		Json::Value root;
		Json::Reader reader;
		if (reader.parse(szHTMLContent, root))
		{
			if (root["flag"].asInt() != 0)
				return;

			if(!IsExist(szHTMLContent,"data"))
				return;
		
			if(IsExist(szHTMLContent, "msg_id"))
				pMsg.uMsgID  = root["data"]["msg_id"].asInt();

			if(IsExist(szHTMLContent, "close_show"))
				pMsg.uCloseState = root["data"]["close_show"].asInt();

			if (IsExist(szHTMLContent, "dataRender")
				&& IsExist(szHTMLContent, "text"))
			{
				szMsg = root["data"]["dataRender"]["text"].asString();
			}
			
			if (IsExist(szHTMLContent, "dataRender")
				&& IsExist(szHTMLContent, "xpos"))
			{
				pMsg.uXPos = root["data"]["dataRender"]["xpos"].asInt();
			}

			if (IsExist(szHTMLContent, "dataRender")
				&& IsExist(szHTMLContent, "ypos"))
			{
				pMsg.uYPos = root["data"]["dataRender"]["ypos"].asInt();
			}
		}
	}


	static BOOL PostDataToServer(LPCTSTR  lpUrl, LPCSTR szData)
	{
		CWinHTTPLib http;

		http.SendData(lpUrl, szData);
		std::string szHTMLContent = http.GetBodyText();

		return TRUE;
	}
};


namespace des_base {
	static const std::string base64_chars =
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz"
		"0123456789+/";


	static inline bool is_base64(unsigned char c) {
		return (isalnum(c) || (c == '+') || (c == '/'));
	}

	static std::string base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len) {
		std::string ret;
		int i = 0;
		int j = 0;
		unsigned char char_array_3[3];
		unsigned char char_array_4[4];

		while (in_len--) {
			char_array_3[i++] = *(bytes_to_encode++);
			if (i == 3) {
				char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
				char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
				char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
				char_array_4[3] = char_array_3[2] & 0x3f;

				for (i = 0; (i < 4); i++)
					ret += base64_chars[char_array_4[i]];
				i = 0;
			}
		}

		if (i)
		{
			for (j = i; j < 3; j++)
				char_array_3[j] = '\0';

			char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
			char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
			char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
			char_array_4[3] = char_array_3[2] & 0x3f;

			for (j = 0; (j < i + 1); j++)
				ret += base64_chars[char_array_4[j]];

			while ((i++ < 3))
				ret += '=';

		}
		return ret;
	}


	static std::string base64_decode(std::string const& encoded_string)
	{
		int in_len = encoded_string.size();
		int i = 0;
		int j = 0;
		int in_ = 0;
		unsigned char char_array_4[4], char_array_3[3];
		std::string ret;

		while (in_len-- && (encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
			char_array_4[i++] = encoded_string[in_]; in_++;
			if (i == 4) {
				for (i = 0; i < 4; i++)
					char_array_4[i] = base64_chars.find(char_array_4[i]);

				char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
				char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
				char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

				for (i = 0; (i < 3); i++)
					ret += char_array_3[i];
				i = 0;
			}
		}

		if (i) {
			for (j = i; j < 4; j++)
				char_array_4[j] = 0;

			for (j = 0; j < 4; j++)
				char_array_4[j] = base64_chars.find(char_array_4[j]);

			char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
			char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
			char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

			for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
		}

		return ret;
	}

	static const std::string toHexString(const unsigned char* input, const int datasize)
	{
		std::string output;
		char ch[3];

		for (int i = 0; i < datasize; ++i)
		{
			sprintf_s(ch, 3, "%02x", input[i]);
			output += ch;
		}
		return output;
	}

	static std::string HexToStr(const std::string& str)
	{
		std::string result;
		for (size_t i = 0; i < str.length(); i += 2)
		{
			std::string byte = str.substr(i, 2);
			char chr = (char)(int)strtol(byte.c_str(), NULL, 16);
			result.push_back(chr);
		}
		return result;
	}

};