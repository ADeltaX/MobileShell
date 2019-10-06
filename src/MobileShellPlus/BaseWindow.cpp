﻿#include "pch.h"
#include "BaseWindow.h"
#include <string>
#include <windowsx.h>
#include <ShellScalingApi.h>
#include <tpcshrd.h>
#include <commctrl.h>
#include <winuser.h>

#pragma comment(lib, "SHCore")
#pragma comment(lib, "shell32")

using namespace winrt;
using namespace Windows::Foundation::Numerics;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Markup;
using namespace Windows::UI::Xaml::Hosting;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Composition;
using namespace Windows::UI;


typedef HWND(WINAPI* CreateWindowInBand)(_In_ DWORD dwExStyle, _In_opt_ LPCWSTR lpClassName, _In_opt_ LPCWSTR lpWindowName, _In_ DWORD dwStyle, _In_ int X, _In_ int Y, _In_ int nWidth, _In_ int nHeight, _In_opt_ HWND hWndParent, _In_opt_ HMENU hMenu, _In_opt_ HINSTANCE hInstance, _In_opt_ LPVOID lpParam, DWORD band);
typedef BOOL(WINAPI* SetWindowBand)(HWND hWnd, HWND hwndInsertAfter, DWORD dwBand);
typedef BOOL(WINAPI* GetWindowBand)(HWND hWnd, PDWORD pdwBand);

BaseWindow::BaseWindow(_In_ HINSTANCE hInstance, LPCWSTR str)
{
	InitWindow(hInstance, str);
}

bool BaseWindow::InitWindow(HINSTANCE hInstance, LPCWSTR str)
{
	hwndChild = CreateWindowInternal(hInstance, str);

	SetWindowPos(hwndParent, nullptr, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOZORDER);
	SetWindowPos(hwndChild, nullptr, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOZORDER);

	return true;
}

void BaseWindow::SetXamlContent(const UIElement& element)
{
	_xamlSource.Content(element); //default
	baseElement = element;
}

UIElement BaseWindow::GetXamlContent()
{
	return baseElement;
}

void BaseWindow::SetSize(int width, int height, double dpi)
{
	SetWindowPos(hwndChild, nullptr, 0, 0, width, height, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOSENDCHANGING);
	SetWindowPos(hwndParent, nullptr, 0, 0, width, height, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOSENDCHANGING);
	this->width = width;
	this->height = height;
	this->effectiveDpi = dpi;
}

void BaseWindow::SetPosition(int x, int y)
{
	SetWindowPos(hwndParent, nullptr, x, y, 0, 0, SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOSENDCHANGING);
	this->x = x;
	this->y = y;
}

void BaseWindow::Show()
{
	if (baseElement == nullptr)
		SetXamlContent(BuildUIElement());

	ShowWindow(hwndParent, 5);
	UpdateWindow(hwndParent);
}

LRESULT CALLBACK ParentWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	BaseWindow* wthis;

	if (message == WM_NCCREATE)
	{
		wthis = static_cast<BaseWindow*>(reinterpret_cast<CREATESTRUCT*>(lParam)->lpCreateParams);

		SetLastError(0);
		if (!SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(wthis)))
		{
			if (GetLastError() != 0)
				return FALSE;
		}
	}
	else
	{
		wthis = reinterpret_cast<BaseWindow*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
	}

	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_WINDOWPOSCHANGING:
		return 0;

	case WM_DISPLAYCHANGE:
		if (wthis)
			wthis->OnDisplayChange();
		break;

		//Workaround for startmenu/taskview:
	case WM_MOUSEACTIVATE:
		return MA_NOACTIVATE;

	case WM_TABLET_QUERYSYSTEMGESTURESTATUS:
		return TABLET_DISABLE_PRESSANDHOLD;
		
	case WM_RBUTTONDOWN:
		return -1;

	case WM_DPICHANGED:
		if (wthis)
		{
			const auto dpi = LOWORD(wParam) / 96.0;
			wthis->OnDpiChanged(dpi);
		}
		break;
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}

HWND BaseWindow::CreateWindowInternal(HINSTANCE hInstance, LPCWSTR str)
{
	auto lpszClassName = str;

	WNDCLASS wndParentClass;

	wndParentClass.style = CS_HREDRAW | CS_VREDRAW;
	wndParentClass.lpfnWndProc = ParentWndProc;
	wndParentClass.cbClsExtra = 0;
	wndParentClass.cbWndExtra = sizeof(BaseWindow);
	wndParentClass.hInstance = hInstance;
	wndParentClass.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	wndParentClass.hCursor = LoadCursor(hInstance, IDC_ARROW);
	wndParentClass.hbrBackground = static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
	wndParentClass.lpszMenuName = nullptr;
	wndParentClass.lpszClassName = lpszClassName;

	if (!RegisterClass(&wndParentClass))
	{
		DWORD Error = GetLastError();
		MessageBox(nullptr, TEXT("Error registering window class."), lpszClassName, MB_ICONERROR);
		return nullptr;
	}

	const auto hpath = LoadLibrary(L"user32.dll");
	const auto proc_add = CreateWindowInBand(GetProcAddress(hpath, "CreateWindowInBand"));

	hwndParent = proc_add(WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOREDIRECTIONBITMAP | WS_EX_NOACTIVATE, lpszClassName,
		TEXT("MobShell"),
		0L,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		nullptr,
		nullptr,
		wndParentClass.hInstance,
		this,
		ZBID_DESKTOP);

	if (!hwndParent)
	{
		const auto err = GetLastError();
		const auto test1 = std::to_string(err);
		MessageBoxA(hwndParent, test1.c_str(), "NO", 0);

		exit(0);
	}

	SetWindowLong(hwndParent, GWL_STYLE, 0);
	SetWindowLong(hwndParent, GWL_EXSTYLE, 0);
	ShowWindow(hwndParent, 0);
	UpdateWindow(hwndParent);

	auto interop_detail = _xamlSource.as<IDesktopWindowXamlSourceNative>();
	check_hresult(interop_detail->AttachToWindow(hwndParent));

	HWND int_hw;

	interop_detail->get_WindowHandle(&int_hw);

	SetWindowLong(hwndParent, GWL_EXSTYLE, WS_EX_NOACTIVATE);

	GESTURECONFIG config;
	config.dwID = 0;
	config.dwWant = 0;
	config.dwBlock = GC_ALLGESTURES;
	auto res = SetGestureConfig(int_hw, 0, 1, &config, sizeof(config));
	auto res1 = SetGestureConfig(hwndParent, 0, 1, &config, sizeof(config));

	
	//workaround for "white rectangle" touch
	//ToDo: sub-classing will cause an epic crash. Find another workaround.
		
	return int_hw;
}

UIElement BaseWindow::BuildUIElement()
{
	return Grid();
}