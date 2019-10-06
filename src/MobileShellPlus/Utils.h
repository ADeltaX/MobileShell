#pragma once
#include "pch.h"

enum WinTaskbarState
{
	AutoHide = 1,
	OnTop = 0
};

enum WinTaskbarPosition
{
	TSB_LEFT = 0,
	TSB_TOP = 1,
	TSB_RIGHT = 2,
	TSB_BOTTOM = 3,
};

enum WinTaskbarIconSize
{
	TSB_SMALL = 1,
	TSB_NORMAL = 0,
};

class Utils
{
public:
	static void SendKeyStroke(WORD key);
	static void SendKeyStrokes(WORD key1, WORD key2);
	static UINT RegisterAppBar(HWND hWnd, double width, double height, double dpi, int abedge);
	static void ABSetPos(HWND hWnd, double width, double height, double dpi, int abedge);
	static void OpenTaskView();
	static void RemoveWinGestures();
	static MONITORINFO GetMonitorInfoFromWindow(HWND hWnd);
	static void GetMonitorSizeAndDpiFromWindow(HWND window, double* dpi, int* height, int* width);
	static DWORD GetCurrentOrientation();
	static void SetWinTaskbarState(WinTaskbarState state);
	static void SetWinTaskbarPos(int swp);
	static void SetWinTaskbarPosition(WinTaskbarPosition pos);
	static void SetWinTaskbarIcons(WinTaskbarIconSize size);
	static void SetWinTaskbarSize(int area, int size);
};


