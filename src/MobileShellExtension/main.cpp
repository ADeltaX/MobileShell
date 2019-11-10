#include "pch.h"

using namespace winrt;

#pragma comment(lib, "gdi32.lib")

enum ZBID
{
	ZBID_DEFAULT = 0,
	ZBID_DESKTOP = 1,
	ZBID_UIACCESS = 2,
	ZBID_IMMERSIVE_IHM = 3,
	ZBID_IMMERSIVE_NOTIFICATION = 4,
	ZBID_IMMERSIVE_APPCHROME = 5,
	ZBID_IMMERSIVE_MOGO = 6,
	ZBID_IMMERSIVE_EDGY = 7,
	ZBID_IMMERSIVE_INACTIVEMOBODY = 8,
	ZBID_IMMERSIVE_INACTIVEDOCK = 9,
	ZBID_IMMERSIVE_ACTIVEMOBODY = 10,
	ZBID_IMMERSIVE_ACTIVEDOCK = 11,
	ZBID_IMMERSIVE_BACKGROUND = 12,
	ZBID_IMMERSIVE_SEARCH = 13,
	ZBID_GENUINE_WINDOWS = 14,
	ZBID_IMMERSIVE_RESTRICTED = 15,
	ZBID_SYSTEM_TOOLS = 16,
	ZBID_LOCK = 17,
	ZBID_ABOVELOCK_UX = 18,
};

LRESULT CALLBACK TrashParentWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_WINDOWPOSCHANGING:
		return 0;
	case WM_CLOSE:
		HANDLE myself;
		myself = OpenProcess(PROCESS_ALL_ACCESS, false, GetCurrentProcessId());
		TerminateProcess(myself, 0);

		return true;

		break;

	default:
		break;
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}

HWND hwnd = NULL;

typedef HWND(WINAPI* CreateWindowInBand)(_In_ DWORD dwExStyle, _In_opt_ ATOM atom, _In_opt_ LPCWSTR lpWindowName, _In_ DWORD dwStyle, _In_ int X, _In_ int Y, _In_ int nWidth, _In_ int nHeight, _In_opt_ HWND hWndParent, _In_opt_ HMENU hMenu, _In_opt_ HINSTANCE hInstance, _In_opt_ LPVOID lpParam, DWORD band);
typedef BOOL(WINAPI* SetWindowBand)(HWND hWnd, HWND hwndInsertAfter, DWORD dwBand);
typedef BOOL(WINAPI* GetWindowBand)(HWND hWnd, PDWORD pdwBand);
typedef HDWP (WINAPI* DeferWindowPosAndBand)(_In_ HDWP hWinPosInfo, _In_ HWND hWnd, _In_opt_ HWND hWndInsertAfter, _In_ int x, _In_ int y, _In_ int cx, _In_ int cy, _In_ UINT uFlags, DWORD band, DWORD pls);

typedef BOOL(WINAPI* SetBrokeredForeground)(HWND hWnd);
#define  __imp_SetBrokeredForeground 2522

void CreateWin(HMODULE hModule, UINT zbid, const wchar_t* title, const wchar_t* classname)
{
	{
		HINSTANCE hInstance = hModule;

		WNDCLASSEX wndParentClass;

		wndParentClass.cbSize = 80;
		wndParentClass.cbClsExtra = 0;
		wndParentClass.hIcon = NULL;
		wndParentClass.lpszMenuName = NULL;
		wndParentClass.hIconSm = NULL;
		wndParentClass.lpfnWndProc = TrashParentWndProc;
		wndParentClass.hInstance = hInstance;
		wndParentClass.style = CS_HREDRAW | CS_VREDRAW;
		wndParentClass.hCursor = LoadCursor(0, IDC_ARROW);
		wndParentClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
		wndParentClass.lpszClassName = classname;
		
		auto res = RegisterClassEx(&wndParentClass);

		const auto hpath = LoadLibrary(L"user32.dll");
		const auto pCreateWindowInBand = CreateWindowInBand(GetProcAddress(hpath, "CreateWindowInBand"));
		const auto pSetWindowBand = SetWindowBand(GetProcAddress(hpath, "SetWindowBand"));
		const auto pSetBrokeredForeground = SetBrokeredForeground(GetProcAddress(hpath, MAKEINTRESOURCEA(__imp_SetBrokeredForeground)));

		auto hwndParent = pCreateWindowInBand(WS_EX_TOPMOST | WS_EX_NOACTIVATE,
			res,
			NULL,
			0x80000000,
			0, 0, 0, 0,
			NULL,
			NULL,
			wndParentClass.hInstance,
			LPVOID(res),
			zbid);

		HRGN hrg = CreateRoundRectRgn(0, 0, 600, 600, 8, 8);
		SetWindowRgn(hwndParent, hrg, true);

		//pSetBrokeredForeground(hwndParent); //Works only if the window is created in ZBID_GENUINE_WINDOWS band.
		//pSetWindowBand(hwndParent, HWND_TOPMOST, ZBID_ABOVELOCK_UX); //This still doesn't in any case.


		SetWindowLong(hwndParent, GWL_STYLE, 0);
		SetWindowLong(hwndParent, GWL_EXSTYLE, 0);

		SetWindowPos(hwndParent, nullptr, 40, 40, 600, 600, SWP_SHOWWINDOW | SWP_NOZORDER);
		ShowWindow(hwndParent, SW_SHOW);
		UpdateWindow(hwndParent);

		if (hwndParent != nullptr)
			hwnd = hwndParent;
	}
}

DWORD WINAPI UwU(LPVOID lpParam)
{
	init_apartment();

	//MessageBox(NULL, L"NOT RIP", L"OK", 0);
	//auto windows_xaml_manager = WindowsXamlManager::InitializeForCurrentThread();
	//MessageBox(NULL, L"RIP", L"OK", 0);
	
	CreateWin(NULL, ZBID_SYSTEM_TOOLS, L"Really Genuine Window++", L"TestPlus");

	auto res = BeginDeferWindowPos(1);

	if (!res)
	{
		MessageBox(NULL, L"RIP1", L"OK", 0);
	}

	const auto hpath = LoadLibrary(L"user32.dll");
	const auto pDeferWindowPosAndBand = DeferWindowPosAndBand(GetProcAddress(hpath, "DeferWindowPosAndBand"));

	res = pDeferWindowPosAndBand(res, hwnd, HWND_TOPMOST, 20, 20, 600, 600, 0, ZBID_DESKTOP, 1);

	if (!res)
	{
		wchar_t buf[256];
		FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			buf, (sizeof(buf) / sizeof(wchar_t)), NULL);

		MessageBox(NULL, buf, L"RIP_DeferWindowPosAndBand", 0);
	}
	else
		EndDeferWindowPos(res);

	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule,
					  DWORD  ul_reason_for_call,
					  LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		CreateThread(nullptr, 0, UwU, hModule, NULL, NULL);
	break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}

	return TRUE;
}

