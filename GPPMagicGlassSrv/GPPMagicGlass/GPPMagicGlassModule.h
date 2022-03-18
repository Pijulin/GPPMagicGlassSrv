#pragma once
#include <windows.h>
#include <string>

using namespace std;

namespace GPPMagicGlassModule
{
	// LoadLibrary
	void LoadModule();
	
	// ��ȡGPU�б�
	string GetGPUList();

	// ��ʼ��
	BOOL InitApp();

	// ������־
	BOOL SetLog(UINT logLevel);

	// ����
	string Run(
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
		int SimulateExclusiveFullscreen,	// ģ���ռȫ��
		int disableVsync						// ��ֱͬ�� ��YES/NO��
	);

	void Close();
}