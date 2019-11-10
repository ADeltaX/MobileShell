#include "pch.h"

#include <shellapi.h>
#include "Utils.h"
#include <ShellScalingApi.h>
#include <Winuser.h>

#pragma comment(lib, "shell32")

MONITORINFO Utils::GetMonitorInfoFromWindow(HWND hWnd)
{
	const HMONITOR monitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
	MONITORINFO info;
	info.cbSize = sizeof(MONITORINFO);
	GetMonitorInfo(monitor, &info);

	return info;
}

void Utils::GetMonitorSizeAndDpiFromWindow(HWND window, double* dpi, int* height, int* width)
{
	HMONITOR monitor = MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST);
	MONITORINFO info;
	info.cbSize = sizeof(MONITORINFO);
	GetMonitorInfo(monitor, &info);
	*width = info.rcMonitor.right - info.rcMonitor.left;
	*height = info.rcMonitor.bottom - info.rcMonitor.top;

	//Get DPI
	unsigned int dpiX = 0;
	unsigned int dpiY = 0;
	GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY);

	*dpi = dpiX / double(96);
}

DWORD Utils::GetCurrentOrientation()
{
	DEVMODE devmode = {};
	devmode.dmSize = sizeof(DEVMODE);
	devmode.dmFields = DM_DISPLAYORIENTATION;

	//Check display orientation
	EnumDisplaySettingsEx(NULL, ENUM_CURRENT_SETTINGS, &devmode, EDS_RAWMODE);

	return devmode.dmDisplayOrientation;
}

bool Utils::SystemUsesLightTheme()
{
	unsigned long dwData = 0;
	unsigned long cbData = sizeof(unsigned long);

	HKEY hThemeKey;

	long error = RegOpenKeyEx(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", NULL, KEY_READ, &hThemeKey);
	if (error == ERROR_SUCCESS)
	{
		long errKey = RegQueryValueEx(hThemeKey, L"SystemUsesLightTheme", 0, 0, LPBYTE(&dwData), &cbData);

		if (error == ERROR_SUCCESS)
		{
			return (bool)dwData;
		}
		else
		{
			RegCloseKey(hThemeKey);
			return true;
		}
	}
	else
	{
		return true;
	}
}