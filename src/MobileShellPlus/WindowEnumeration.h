#pragma once
#include "pch.h"
#include <dwmapi.h>

struct Window32
{
public:
	Window32(nullptr_t) {}
	Window32(HWND hwnd, std::wstring const& title, std::wstring& className)
	{
		m_hwnd = hwnd;
		m_title = title;
		m_className = className;
	}

	HWND Hwnd() const noexcept { return m_hwnd; }
	std::wstring Title() const noexcept { return m_title; }
	std::wstring ClassName() const noexcept { return m_className; }

private:
	HWND m_hwnd;
	std::wstring m_title;
	std::wstring m_className;
};

std::wstring GetClassName(HWND hwnd)
{
	std::array<WCHAR, 1024> className;

	::GetClassName(hwnd, className.data(), (int)className.size());

	std::wstring title(className.data());
	return title;
}

std::wstring GetWindowText(HWND hwnd)
{
	std::array<WCHAR, 1024> windowText;

	::GetWindowText(hwnd, windowText.data(), (int)windowText.size());

	std::wstring title(windowText.data());
	return title;
}

bool IsAltTabWindow(Window32 const& window)
{
	HWND hwnd = window.Hwnd();
	HWND shellWindow = GetShellWindow();

	auto title = window.Title();
	auto className = window.ClassName();

	if (hwnd == shellWindow)
	{
		return false;
	}

	if (title.length() == 0)
	{
		return false;
	}

	if (!IsWindowVisible(hwnd))
	{
		return false;
	}

	if (GetAncestor(hwnd, GA_ROOT) != hwnd)
	{
		return false;
	}

	LONG style = GetWindowLong(hwnd, GWL_STYLE);
	if (!((style & WS_DISABLED) != WS_DISABLED))
	{
		return false;
	}

	LONG exstyle = GetWindowLong(hwnd, GWL_EXSTYLE);
	if (!((exstyle & WS_EX_TOOLWINDOW) != WS_EX_TOOLWINDOW))
	{
		return false;
	}

	DWORD cloaked = FALSE;
	HRESULT hrTemp = DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked));
	if (SUCCEEDED(hrTemp) &&
		cloaked == DWM_CLOAKED_SHELL)
	{
		if (className.compare(L"ApplicationFrameWindow"))
			return false;
	}

	return true;
}

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	auto class_name = GetClassName(hwnd);
	auto title = GetWindowText(hwnd);

	auto window = Window32(hwnd, title, class_name);

	if (!IsAltTabWindow(window))
	{
		return TRUE;
	}

	std::vector<Window32>& windows = *reinterpret_cast<std::vector<Window32>*>(lParam);
	windows.push_back(window);

	return TRUE;
}

const std::vector<Window32> EnumerateWindows()
{
	std::vector<Window32> windows;
	EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&windows));

	if (!windows.empty())
	{
		
	}
	
	return windows;
}