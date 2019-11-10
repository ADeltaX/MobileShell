#include "pch.h"

#include <shellapi.h>
#include "Utils.h"
#include <ShellScalingApi.h>
#include <Winuser.h>

#pragma comment(lib, "shell32")

void Utils::SendKeyStroke(WORD key)
{
	INPUT inputs[2];

	inputs[0].type = INPUT_KEYBOARD;
	inputs[0].ki.time = 0;
	inputs[0].ki.wScan = 0;
	inputs[0].ki.dwExtraInfo = GetMessageExtraInfo();
	inputs[0].ki.wVk = key;
	inputs[0].ki.dwFlags = 0;

	inputs[1].type = INPUT_KEYBOARD;
	inputs[1].ki.time = 0;
	inputs[1].ki.wScan = 0;
	inputs[1].ki.dwExtraInfo = GetMessageExtraInfo();
	inputs[1].ki.wVk = key;
	inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;

	SendInput(2, inputs, sizeof(INPUT));
}

void Utils::SendKeyStrokes(WORD key1, WORD key2)
{
	INPUT inputs[4];

	inputs[0].type = INPUT_KEYBOARD;
	inputs[0].ki.time = 0;
	inputs[0].ki.wScan = 0;
	inputs[0].ki.dwExtraInfo = GetMessageExtraInfo();
	inputs[0].ki.wVk = key1;
	inputs[0].ki.dwFlags = 0;

	inputs[1].type = INPUT_KEYBOARD;
	inputs[1].ki.time = 0;
	inputs[1].ki.wScan = 0;
	inputs[1].ki.dwExtraInfo = GetMessageExtraInfo();
	inputs[1].ki.wVk = key2;
	inputs[1].ki.dwFlags = 0;

	inputs[2].type = INPUT_KEYBOARD;
	inputs[2].ki.time = 0;
	inputs[2].ki.wScan = 0;
	inputs[2].ki.dwExtraInfo = GetMessageExtraInfo();
	inputs[2].ki.wVk = key2;
	inputs[2].ki.dwFlags = KEYEVENTF_KEYUP;

	inputs[3].type = INPUT_KEYBOARD;
	inputs[3].ki.time = 0;
	inputs[3].ki.wScan = 0;
	inputs[3].ki.dwExtraInfo = GetMessageExtraInfo();
	inputs[3].ki.wVk = key1;
	inputs[3].ki.dwFlags = KEYEVENTF_KEYUP;

	SendInput(4, inputs, sizeof(INPUT));
}

UINT Utils::RegisterAppBar(HWND hWnd, double width, double height, double dpi, int abedge)
{
	APPBARDATA abd = {0};
	abd.cbSize = sizeof(APPBARDATA);
	abd.hWnd = hWnd;
	UINT uCallback = RegisterWindowMessage(L"AppBarMessage");
	abd.uCallbackMessage = uCallback;

	SHAppBarMessage(ABM_NEW, &abd);
	ABSetPos(hWnd, width, height, dpi, abedge);

	return uCallback;
}

UINT Utils::UnregisterAppBar(HWND hWnd)
{
	APPBARDATA abd = { 0 };
	abd.cbSize = sizeof(APPBARDATA);
	abd.hWnd = hWnd;

	SHAppBarMessage(ABM_REMOVE, &abd);
	return -1;
}

void Utils::ABSetPos(HWND hWnd, double width, double height, double dpi, int abedge)
{
	APPBARDATA abd = {0};
	abd.cbSize = sizeof(APPBARDATA);
	abd.hWnd = hWnd;
	abd.uEdge = abedge;
	int sWidth = std::ceil(width);
	int sHeight = std::ceil(height);

	MONITORINFO monitorInfo = GetMonitorInfoFromWindow(hWnd);
	
	int top = monitorInfo.rcMonitor.top;
	int left = monitorInfo.rcMonitor.left;
	int right = monitorInfo.rcMonitor.right;
	int bottom = monitorInfo.rcMonitor.bottom;

	if (abd.uEdge == ABE_LEFT || abd.uEdge == ABE_RIGHT)
	{
		abd.rc.top = top;
		abd.rc.bottom = bottom;
		if (abd.uEdge == ABE_LEFT)
		{
			abd.rc.left = left;
			abd.rc.right = abd.rc.right + sWidth;
		}
		else
		{
			abd.rc.right = right;
			abd.rc.left = abd.rc.right - sWidth;
		}

	}
	else
	{
		abd.rc.left = left;
		abd.rc.right = right;
		if (abd.uEdge == ABE_TOP)
		{
			abd.rc.top = top;
			abd.rc.bottom = abd.rc.top + sHeight;
		}
		else
		{
			abd.rc.bottom = bottom;
			abd.rc.top = abd.rc.bottom - sHeight;
		}
	}

	SHAppBarMessage(ABM_SETPOS, &abd);
}

void Utils::ClickTaskView()
{
	//const auto hWnd_shell = FindWindow(L"Shell_TrayWnd", L"");

	//const auto hWnd_task_view = FindWindowEx(hWnd_shell, nullptr, L"TrayButton", L"Task View");
	//if (hWnd_task_view != nullptr)
	//{
	//	SendMessage(hWnd_task_view, WM_LBUTTONDOWN, 0, 0);
	//	SendMessage(hWnd_task_view, WM_LBUTTONUP, 0, 0);
	//	return;
	//}

	SendKeyStrokes(VK_LWIN, VK_TAB);


	//Still doesn't fix the "X" that disappears
	//SHELLEXECUTEINFO exec = {};
	//exec.cbSize = sizeof(SHELLEXECUTEINFO);
	//exec.hwnd = nullptr;
	//exec.lpFile = L"explorer.exe";
	//exec.lpParameters = L"shell:::{3080F90E-D7AD-11D9-BD98-0000947B0257}";

	//ShellExecuteEx(&exec);
}

void Utils::ClickBack()
{
	/*const auto hWnd_shell = FindWindow(L"Shell_TrayWnd", L"");

	const auto hWnd_back = FindWindowEx(hWnd_shell, nullptr, L"TrayButton", L"Back");
	if (hWnd_back != nullptr)
	{
		SendMessage(hWnd_back, WM_LBUTTONDOWN, 0, 0);
		SendMessage(hWnd_back, WM_LBUTTONUP, 0, 0);
		return;
	}*/

	Utils::SendKeyStrokes(VK_LWIN, VK_BACK);
}

void Utils::ClickStartMenu()
{
	/*const auto hWnd_shell = FindWindow(L"Shell_TrayWnd", L"");

	const auto hWnd_start = FindWindowEx(hWnd_shell, nullptr, L"Start", L"Start");
	if (hWnd_start != nullptr)
	{
		SendMessage(hWnd_start, WM_LBUTTONDOWN, 0, 0);
		SendMessage(hWnd_start, WM_LBUTTONUP, 0, 0);
		return;
	}*/

	SendKeyStroke(VK_LWIN);
}

void Utils::ClickSearch()
{
	//const auto hWnd_shell = FindWindow(L"Shell_TrayWnd", L"");

	//const auto hWnd_back = FindWindowEx(hWnd_shell, nullptr, L"TrayButton", L"Type here to search");
	//if (hWnd_back != nullptr)
	//{
	//	SendMessage(hWnd_back, WM_LBUTTONDOWN, 0, 0);
	//	SendMessage(hWnd_back, WM_LBUTTONUP, 0, 0);
	//	return;
	//}

	Utils::SendKeyStrokes(VK_LWIN, 0x51); //KEY S
}

void Utils::RemoveWinGestures()
{
	const auto EdgeUiInputTopHwnd = FindWindow(L"EdgeUiInputTopWndClass", L"");
	const auto EdgeUiInputHwnd = FindWindow(L"EdgeUiInputWndClass", L"");

	//TEST
	ShowWindow(EdgeUiInputTopHwnd, SW_HIDE);
	ShowWindow(EdgeUiInputHwnd, SW_HIDE);
}

void Utils::SetWinTaskbarState(WinTaskbarState state)
{
	APPBARDATA abd;
	abd.cbSize = sizeof(APPBARDATA);
	abd.hWnd = FindWindow(L"Shell_TrayWnd", L"");

	abd.lParam = state;
	SHAppBarMessage(ABM_SETSTATE, &abd);

	return;

	//This currently doesn't work as UWP packaged.
	HKEY hKey;

	RegOpenKeyEx(HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced", 0, KEY_ALL_ACCESS, &hKey);
	DWORD value = 0;

	switch (state)
	{
	case AutoHide:
		value = 1;
		break;
	case OnTop:
		value = 0;
		break;
	default:
		break;
	}

	RegSetValueEx(hKey, L"TaskbarAutoHideInTabletMode", 0, REG_DWORD, (const BYTE*)&value, sizeof(value));
	RegCloseKey(hKey);

	SendNotifyMessage(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM)L"TraySettings");
}

void Utils::SetWinTaskbarVisible(bool visible)
{
	//Far better than simply hiding the window. This way will prevent unwanted and random "popups"

	const auto taskbarHwnd = FindWindow(L"Shell_TrayWnd", L"");
	
	if (visible)
	{
		SetWindowLong(taskbarHwnd, GWL_EXSTYLE, GetWindowLong(taskbarHwnd, GWL_EXSTYLE) & ~(WS_EX_LAYERED | WS_EX_COMPOSITED));
		SetLayeredWindowAttributes(taskbarHwnd, RGB(0, 0, 0), 255, LWA_ALPHA);
	}
	else
	{
		SetWindowLong(taskbarHwnd, GWL_EXSTYLE, GetWindowLong(taskbarHwnd, GWL_EXSTYLE) | WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_COMPOSITED);
		SetLayeredWindowAttributes(taskbarHwnd, RGB(0, 0, 0), 0, LWA_ALPHA);
	}

}

void Utils::SetWinTaskbarPosition(WinTaskbarPosition pos)
{
	//Issue: this will change the taskbar position in ALL MONITORS
	//I wasn't able to find how to change on primary taskbar only, unfortunately
	//ALSO this makes the rotation a lot slower (because APPBAR & GDI slowness technology™)
	//
	//Should we enable this?

	//HWND htaskbar = FindWindow(L"Shell_TrayWnd", NULL);
	//SendNotifyMessage(htaskbar, 0x05CA, 6, (LPARAM)pos);
}

void Utils::SetWinTaskbarIcons(WinTaskbarIconSize size)
{
	HKEY hKey;
	
	RegOpenKeyEx(HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced", 0, KEY_ALL_ACCESS, &hKey);
	DWORD value = int(size);
	RegSetValueEx(hKey, L"TaskbarSmallIcons", 0, REG_DWORD, (const BYTE*)&value, sizeof(value));
	RegCloseKey(hKey);
	
	SendNotifyMessage(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM)L"TraySettings");
}

void Utils::SetWinTaskbarSize(int area, int size)
{
	//TODO

	HWND hTaskbarWnd = FindWindow(L"Shell_TrayWnd", NULL);
	if (hTaskbarWnd)
	{
		RECT rc;
		GetWindowRect(hTaskbarWnd, &rc);

		if (area == 1)
			rc.left = rc.right - size;
		else
			rc.right = rc.left + size;

		SendMessage(hTaskbarWnd, WM_SIZING, WMSZ_LEFT, (LPARAM)&rc);
		SetWindowPos(hTaskbarWnd, NULL, 0, 0, size, rc.bottom - rc.top, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
	}
}

void Utils::SetEdgeSwipeStatus(bool enabled)
{
	//Don't use this as it needs a logout........

	HKEY hKey;

	RegOpenKeyEx(HKEY_CURRENT_USER, L"Software\\Policies\\Microsoft\\Windows\\EdgeUI", 0, KEY_ALL_ACCESS, &hKey);
	DWORD value = int(enabled);
	RegSetValueEx(hKey, L"AllowEdgeSwipe", 0, REG_DWORD, (const BYTE*)&value, sizeof(value));

	SendNotifyMessage(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM)L"TraySettings");
	
	RegCloseKey(hKey);
}

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