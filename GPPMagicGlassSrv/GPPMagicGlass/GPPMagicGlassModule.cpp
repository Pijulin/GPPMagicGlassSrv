#include "GPPMagicGlassModule.h"
#include <LOG/Log.h>
#include "Utils.h"

enum class FlagMasks : UINT {
	NoCursor = 0x1,
	AdjustCursorSpeed = 0x2,
	SaveEffectSources = 0x4,
	SimulateExclusiveFullscreen = 0x8,
	DisableLowLatency = 0x10,
	BreakpointMode = 0x20,
	DisableWindowResizing = 0x40,
	DisableDirectFlip = 0x80,
	ConfineCursorIn3DGames = 0x100,
	CropTitleBarOfUWP = 0x200,
	DisableEffectCache = 0x400,
	DisableVSync = 0x800,
	WarningsAreErrors = 0x1000
};

const UINT WM_DESTORYHOST = RegisterWindowMessage(L"GPPMagicGlass_WM_DESTORYHOST");

static constexpr const wchar_t* HOST_WINDOW_CLASS_NAME = L"Window_GPPMagicGlass_HOST";
static constexpr const wchar_t* DDF_WINDOW_CLASS_NAME = L"Window_GPPMagicGlass_DDF";
static constexpr const wchar_t* HOST_WINDOW_TITLE = L"GPPMagicGlass";

typedef const char*(__stdcall* PFN_GETALLGRAPHICSADATERS)();

typedef bool(__stdcall* PFN_INITAPP)(UINT logLevel, const char* logFileName, int logArchiveAboveSize, int logMaxArchiveFiles);

typedef void(__stdcall* PFN_SETLOG)(UINT logLevel);

typedef const char*(__stdcall* PFN_RUN)(
	HWND hwndSrc,
	const char* effectsJson,
	UINT flags,
	UINT captureMode,
	float cursorZoomFactor,				// ������ 0����Դ������ͬ�����������ű���
	UINT cursorInterpolationMode,		// 0������ڣ�1��˫����
	unsigned long long adapterLUID,		// �Կ�LUID
	UINT multiMonitorUsage,				// 0����� 1���ཻ 2������
	UINT cropLeft,
	UINT cropTop,
	UINT cropRight,
	UINT cropBottom
);

static PFN_GETALLGRAPHICSADATERS pfnGETALLGRAPHICSADATERS = NULL;
static PFN_INITAPP pfnINITAPP = NULL;
static PFN_SETLOG pfnSETLOG = NULL;
static PFN_RUN pfnRUN = NULL;

// LoadLibrary
void GPPMagicGlassModule::LoadModule() {
	HMODULE hGPPMGModule = LoadLibrary(L"GPPMagicGlassRT.dll");
	if (hGPPMGModule) {
		pfnGETALLGRAPHICSADATERS = (PFN_GETALLGRAPHICSADATERS)GetProcAddress(hGPPMGModule, "GetAllGraphicsAdapters");
		pfnINITAPP = (PFN_INITAPP)GetProcAddress(hGPPMGModule, "Initialize");
		pfnSETLOG = (PFN_SETLOG)GetProcAddress(hGPPMGModule, "SetLogLevel");
		pfnRUN = (PFN_RUN)GetProcAddress(hGPPMGModule, "Run");
	}
	else {
		LOGINFO(L"Load Module Failed!!!");
	}
}

// ��ȡGPU�б�
string GPPMagicGlassModule::GetGPUList() {
	if (pfnGETALLGRAPHICSADATERS) {
		return pfnGETALLGRAPHICSADATERS();
	}
	else {
		LOGINFO(L"Func Not Found!");
		return "";
	}
}

static BOOL GlobalAppInitFlag = FALSE;

// ��ʼ��
BOOL GPPMagicGlassModule::InitApp() {
	if (GlobalAppInitFlag) {
		LOGINFO(L"App Inited!");
		return TRUE;
	}

	if (pfnINITAPP) {
		BOOL result = pfnINITAPP(6, ".\\logs\\GPPMagicGlassRuntime.log", 4096 * 1024, 4096 * 1024);
		if (!result) {
			LOGINFO(L"Init Failed!");
		}
		else {
			GlobalAppInitFlag = TRUE;
		}
		return result;
	}
	else {
		LOGINFO(L"Func Not Found!");
		return FALSE;
	}
}

// ������־
BOOL GPPMagicGlassModule::SetLog(UINT logLevel) {
	if (!GlobalAppInitFlag) {
		LOGINFO(L"App Not init!");
		return FALSE;
	}

	if (pfnSETLOG) {
		pfnSETLOG(logLevel);
	}
	else {
		LOGINFO(L"Func Not Found!");
		return FALSE;
	}
	return TRUE;
}

// ����
string GPPMagicGlassModule::Run(
	UINT pId,							// ��Ϸ����PID
	int effectMode,						// 0 FSR, 1 FSRDX10�Կ�����ģʽ
	int captureMode,					// 0 Graphics Capture, 1 Desktop Duplication, 2 GDI, 3 Dwm Shared Surface
	float cursorZoom,					// ������Ÿ����� 0����Դԭ������ͬ�����������ű���0.5, 0.75, 1.0, 1.25, 1.5, 2, 3
	int cursorInterpolationMode,		// 0������ڣ�1��˫����
	unsigned long adapterLUID,			// ��ѡ����Ϸ����ʹ�õ��Կ���LUID
	int multiMonitor,					// 0����� 1���ཻ 2������
	int DisableWindowResizing,			// ����ʱ���ô��ڴ�С������YES/NO��
	int CropTitleBarOfUWP,				// �ü�UWP���ڵı�������YES/NO��
	int DisableLowLatency,				// ���������ӳ���������ܡ�YES/NO��
	int NoCursor,						// �����ƹ�꡾YES/NO��	
	int AdjustCursorSpeed,				// ����ʱ��������ٶȡ�YES/NO��
	int ConfineCursorIn3DGames,			// ��3D��Ϸ�����ƹ�꡾YES/NO��
	int DisableDirectFlip,				// ����DirectFlip��YES/NO��
	int SimulateExclusiveFullscreen,	// ģ���ռȫ����YES/NO��
	int disableVsync					// �رմ�ֱͬ�� ��YES/NO��
) {
	if (!GlobalAppInitFlag) {
		LOGINFO(L"App Not init!");
		return "App Not Init";
	}

	if (pfnRUN) {
		// 1. ͨ��PID��ȡ����
		HWND winHWND = Utils::GetWindowHwndByPID(pId);
		if (!winHWND) {
			LOGINFO(L"Window Not Found!");
			return "Window Not Found";
		}

		string effectJSON = "";
		if (effectMode == 0) {
			// Ĭ��
			effectJSON = "[{\"effect\":\"FSR_EASU\",\"scale\":[-1,-1]},{\"effect\":\"FSR_RCAS\",\"sharpness\":0.75}]";
		}
		else {
			// ����
			effectJSON = "[{\"effect\":\"Bicubic\",\"scale\":[-1,-1]},{\"effect\":\"FSR_RCAS\",\"sharpness\":0.75}]";
		}

		UINT flag = (DisableWindowResizing ? (UINT)FlagMasks::DisableWindowResizing : 0) |
			(CropTitleBarOfUWP ? (UINT)FlagMasks::CropTitleBarOfUWP : 0) |
			(DisableLowLatency ? (UINT)FlagMasks::DisableLowLatency : 0) |
			(NoCursor ? (UINT)FlagMasks::NoCursor : 0) |
			(AdjustCursorSpeed ? (UINT)FlagMasks::AdjustCursorSpeed : 0) |
			(ConfineCursorIn3DGames ? (UINT)FlagMasks::ConfineCursorIn3DGames : 0) |
			(DisableDirectFlip ? (UINT)FlagMasks::DisableDirectFlip : 0) |
			(SimulateExclusiveFullscreen ? (UINT)FlagMasks::SimulateExclusiveFullscreen : 0) |
			(disableVsync ? (UINT)FlagMasks::DisableVSync : 0);

		return pfnRUN(
			winHWND,
			effectJSON.c_str(),
			flag,
			captureMode, 
			cursorZoom, 
			cursorInterpolationMode, 
			adapterLUID, 
			multiMonitor, 
			0, 0, 0, 0); // �ݲ�֧���Զ�������
	}
	else {
		LOGINFO(L"Func Not Found!");
		return "Func Not Found";
	}
}

void GPPMagicGlassModule::Close() {
	if (!GlobalAppInitFlag) {
		LOGINFO(L"App Not init!");
		return;
	}

	HWND hHOST = FindWindow(HOST_WINDOW_CLASS_NAME, HOST_WINDOW_TITLE);
	HWND hDDF = FindWindow(DDF_WINDOW_CLASS_NAME, HOST_WINDOW_TITLE);

	::PostMessage(HWND_BROADCAST, WM_DESTORYHOST, 0, 0);

	if (hDDF) {
		::PostMessage(hDDF, WM_QUIT, 0, 0);
	}

	if (hHOST) {
		::PostMessage(hHOST, WM_QUIT, 0, 0);
	}
}
