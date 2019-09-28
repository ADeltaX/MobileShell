#pragma once
#include "pch.h"

#pragma once
class Utils
{
public:
	static void SendKeyStroke(WORD key);
	static void SendKeyStrokes(WORD key1, WORD key2);
	static UINT RegisterAppBar(HWND hWnd, double width, double height, double dpi, int abedge);
	static void ABSetPos(HWND hWnd, double width, double height, double dpi, int abedge);
	static void OpenTaskView();
	static MONITORINFO GetMonitorInfoFromWindow(HWND hWnd);
	static void GetMonitorSizeAndDpiFromWindow(HWND window, double* dpi, int* height, int* width);
	static DWORD GetCurrentOrientation();
	static void SetWinTaskbarState(WinTaskbarState state);
	static void SetWinTaskbarPos(int swp);
};


