#pragma once
#include "pch.h"

class Utils
{
public:
	static MONITORINFO GetMonitorInfoFromWindow(HWND hWnd);
	static void GetMonitorSizeAndDpiFromWindow(HWND window, double* dpi, int* height, int* width);
	static DWORD GetCurrentOrientation();
	static bool SystemUsesLightTheme();
};


