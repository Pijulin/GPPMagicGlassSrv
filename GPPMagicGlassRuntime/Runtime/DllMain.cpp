// Copyright (c) 2021 - present, Liu Xu
//
//  This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.


#include "pch.h"
#include "App.h"
#include "Utils.h"
#include "StrUtils.h"
#include "Logger.h"


#define API_DECLSPEC extern "C" __declspec(dllexport)

static HINSTANCE hInst = NULL;


// DLL 入口
BOOL APIENTRY DllMain(
	HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
) {
	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH:
		hInst = hModule;
		break;
	case DLL_PROCESS_DETACH:
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}

// 日志级别：
// 0：TRACE，1：DEBUG，...，6：OFF
API_DECLSPEC void WINAPI SetLogLevel(UINT logLevel) {
	Logger::Get().SetLevel((spdlog::level::level_enum)logLevel);
}


API_DECLSPEC BOOL WINAPI Initialize(
	UINT logLevel,
	const char* logFileName,
	int logArchiveAboveSize,
	int logMaxArchiveFiles
) {
	// 初始化日志
	if (!Logger::Get().Initialize(logLevel, logFileName, logArchiveAboveSize, logMaxArchiveFiles)) {
		return FALSE;
	}

	// 初始化 App
	if (!App::Get().Initialize(hInst)) {
		return FALSE;
	}

	// 初始化 Hasher
	if (!Utils::Hasher::Get().Initialize()) {
		return FALSE;
	}

	return TRUE;
}

API_DECLSPEC const char* WINAPI Run(
	HWND hwndSrc,
	const char* effectsJson,
	UINT flags,
	UINT captureMode,
	float cursorZoomFactor,				// 负数和 0：和源窗口相同，正数：缩放比例
	UINT cursorInterpolationMode,		// 0：最近邻，1：双线性
	unsigned long long adapterLUID,		// 显卡LUID
	UINT multiMonitorUsage,				// 0：最近 1：相交 2：所有
	UINT cropLeft,
	UINT cropTop,
	UINT cropRight,
	UINT cropBottom
) {
	Logger& logger = Logger::Get();

	if (!hwndSrc || !IsWindow(hwndSrc)) {
		logger.Critical("非法的源窗口句柄");
		return ErrorMessages::GENERIC;
	}

	const auto& version = Utils::GetOSVersion();
	logger.Info(fmt::format("OS 版本：{}.{}.{}",
		version.dwMajorVersion, version.dwMinorVersion, version.dwBuildNumber));

	int len = GetWindowTextLength(hwndSrc);
	if (len <= 0) {
		logger.Info("源窗口无标题");
	} else {
		std::wstring title(len, 0);
		if (!GetWindowText(hwndSrc, &title[0], int(title.size() + 1))) {
			Logger::Get().Error("获取源窗口标题失败");
		} else {
			Logger::Get().Info(StrUtils::Concat("源窗口标题：", StrUtils::UTF16ToUTF8(title)));
		}
	}

	// 根据LUID获取adapterIdx
	UINT adapterIdx = 0;

	winrt::com_ptr<IDXGIFactory1> dxgiFactory;
	HRESULT hr = CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory));
	if (FAILED(hr)) {
		return "CreateDXGIFactory1 ERROR";
	}

	winrt::com_ptr<IDXGIAdapter1> adapter;
	for (UINT adapterIndex = 0; SUCCEEDED(dxgiFactory->EnumAdapters1(adapterIndex, adapter.put())); adapterIndex++) {
		DXGI_ADAPTER_DESC1 desc;
		HRESULT hr = adapter->GetDesc1(&desc);
		if (SUCCEEDED(hr) && desc.Flags != DXGI_ADAPTER_FLAG_SOFTWARE) {
			if (desc.AdapterLuid.LowPart == adapterLUID) {
				adapterIdx = adapterIndex;
				break;
			}
		}
	}

	App& app = App::Get();
	if (!app.Run(hwndSrc, effectsJson, captureMode,
		cursorZoomFactor, cursorInterpolationMode, adapterIdx, multiMonitorUsage,
		RECT{(LONG)cropLeft, (LONG)cropTop, (LONG)cropRight, (LONG)cropBottom}, flags)
	) {
		// 初始化失败
		Logger::Get().Info("App.Run 失败");
		return app.GetErrorMsg();
	}

	logger.Info("即将退出");
	logger.Flush();

	return "";
}


// ----------------------------------------------------------------------------------------
// 以下函数在用户界面的主线程上调用

/**
 * 格式化字符串
 *
 * \param pszFmt 格式描述
 * \param ... 不定参数
 * \return 格式化的结果字符串
 */
std::string format(const char* pszFmt, ...) {
	std::string str;
	va_list args;
	va_start(args, pszFmt);
	{
		int nLength = _vscprintf(pszFmt, args);
		nLength += 1;  //上面返回的长度是包含\0，这里加上
		std::vector<char> chars(nLength);
		_vsnprintf_s(chars.data(), nLength, nLength, pszFmt, args);
		str.assign(chars.data());
	}
	va_end(args);
	return str;
}

#define GAME_PARAM_WINDOW_CLASS_NAME L"GAMEPP_CHECK_DX_SUPPORT_WINDOW_CLASS"

static LRESULT WINAPI DummyMsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	static UINT GPPUnloadMessage = 0;
	switch (msg) {
	case WM_CLOSE:
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_PAINT:
		ValidateRect(hWnd, NULL);       //使窗口有效  

		return 0;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

BOOL CheckSupportDX11(IDXGIAdapter* adap) {
	if (adap == NULL)
		return FALSE;

	//初始化窗口类
	WNDCLASSEX wc = {};

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_CLASSDC;
	wc.lpfnWndProc = DummyMsgProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetModuleHandle(NULL);
	wc.hIcon = NULL;
	wc.hCursor = NULL;
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = GAME_PARAM_WINDOW_CLASS_NAME;
	wc.hIconSm = NULL;

	RegisterClassEx(&wc);           //注册窗口类  

	HWND windowHandle = CreateWindow(
		GAME_PARAM_WINDOW_CLASS_NAME,
		L"GAMEPP_CHECK_DX_SUPPORT_MAIN_WND",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 640, 480
		, NULL
		, NULL
		, wc.hInstance
		, NULL
	);

	DXGI_SWAP_CHAIN_DESC swapDesc;
	ZeroMemory(&swapDesc, sizeof(swapDesc));
	swapDesc.BufferCount = 2;
	swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapDesc.BufferDesc.Width = 2;
	swapDesc.BufferDesc.Height = 2;
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapDesc.OutputWindow = windowHandle;
	swapDesc.SampleDesc.Count = 1;
	swapDesc.Windowed = TRUE;

	IDXGISwapChain* swapChain = NULL;
	ID3D11Device* device = NULL;
	ID3D11DeviceContext* deviceContext = NULL;
	D3D_DRIVER_TYPE driverType = D3D_DRIVER_TYPE_UNKNOWN;
	UINT createDeviceFlags = 0;
	D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0,  D3D_FEATURE_LEVEL_11_1 };
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	BOOL isSupport = TRUE;
	HRESULT hr = D3D11CreateDeviceAndSwapChain(adap, driverType, NULL, createDeviceFlags, featureLevels, numFeatureLevels, D3D11_SDK_VERSION, &swapDesc, &swapChain, &device, NULL, &deviceContext);
	if (FAILED(hr)) {
		isSupport = FALSE;
	}

	if (swapChain)
		swapChain->Release();

	if (device)
		device->Release();

	if (deviceContext)
		deviceContext->Release();

	if (windowHandle) {
		::DestroyWindow(windowHandle);
		windowHandle = NULL;
	}

	UnregisterClass(GAME_PARAM_WINDOW_CLASS_NAME, wc.hInstance);

	return isSupport;
}

BOOL CheckSupportDX10(IDXGIAdapter* adap) {
	if (adap == NULL)
		return FALSE;

	//初始化窗口类
	WNDCLASSEX wc = {};

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_CLASSDC;
	wc.lpfnWndProc = DummyMsgProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetModuleHandle(NULL);
	wc.hIcon = NULL;
	wc.hCursor = NULL;
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = GAME_PARAM_WINDOW_CLASS_NAME;
	wc.hIconSm = NULL;

	RegisterClassEx(&wc);           //注册窗口类  

	HWND windowHandle = CreateWindow(
		GAME_PARAM_WINDOW_CLASS_NAME,
		L"GAMEPP_CHECK_DX_SUPPORT_MAIN_WND",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 640, 480
		, NULL
		, NULL
		, wc.hInstance
		, NULL
	);

	DXGI_SWAP_CHAIN_DESC swapDesc;
	ZeroMemory(&swapDesc, sizeof(swapDesc));
	swapDesc.BufferCount = 2;
	swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapDesc.BufferDesc.Width = 2;
	swapDesc.BufferDesc.Height = 2;
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapDesc.OutputWindow = windowHandle;
	swapDesc.SampleDesc.Count = 1;
	swapDesc.Windowed = TRUE;

	IDXGISwapChain* swapChain = NULL;
	ID3D10Device* device = NULL;
	D3D10_DRIVER_TYPE driverType = D3D10_DRIVER_TYPE_HARDWARE;

	BOOL isSupport = TRUE;
	HRESULT hr = D3D10CreateDeviceAndSwapChain(adap, driverType, NULL, 0, D3D10_SDK_VERSION, &swapDesc, &swapChain, &device);
	if (FAILED(hr)) {
		isSupport = FALSE;
	}

	if (swapChain)
		swapChain->Release();

	if (device)
		device->Release();

	if (windowHandle) {
		::DestroyWindow(windowHandle);
		windowHandle = NULL;
	}

	UnregisterClass(GAME_PARAM_WINDOW_CLASS_NAME, wc.hInstance);

	return isSupport;
}

API_DECLSPEC const char* WINAPI GetAllGraphicsAdapters() {
	static std::string result = "";
	result.clear();

	winrt::com_ptr<IDXGIFactory1> dxgiFactory;
	HRESULT hr = CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory));
	if (FAILED(hr)) {
		return "";
	}

	result += "[";

	winrt::com_ptr<IDXGIAdapter1> adapter;
	for (UINT adapterIndex = 0; SUCCEEDED(dxgiFactory->EnumAdapters1(adapterIndex, adapter.put())); adapterIndex++) {
		DXGI_ADAPTER_DESC1 desc;
		HRESULT hr = adapter->GetDesc1(&desc);
		// 只选择硬件显卡
		if (SUCCEEDED(hr) && desc.Flags != DXGI_ADAPTER_FLAG_SOFTWARE) {
			if (adapterIndex > 0) {
				result += ",";
			}

			IDXGIAdapter* adap = NULL;
			adapter->QueryInterface(IID_PPV_ARGS(&adap));

			UINT GPUDXSupport = 9;

			if (CheckSupportDX11(adap)) {
				GPUDXSupport = 11;
			} else {
				if (CheckSupportDX10(adap)) {
					GPUDXSupport = 10;
				}
			}

			adap->Release();

			std::string GPUName = StrUtils::UTF16ToUTF8(desc.Description);
			UINT64 luid = desc.AdapterLuid.LowPart;

			std::string gpuInfo = format("{\"name\":\"%s\",\"dx\":%lu,\"luid\":%llu}", GPUName.c_str(), GPUDXSupport, luid);
			result += gpuInfo;
		}
	}

	result += "]";

	return result.c_str();
}
