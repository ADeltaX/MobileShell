#pragma once
#include "pch.h"

using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Hosting;

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

#pragma once
class BaseWindow
{

public:
	virtual ~BaseWindow() = default;
	explicit BaseWindow(_In_ HINSTANCE, LPCWSTR str = L"DEF");
	void Show();
	void SetSize(int width, int height, double dpi);
	void SetPosition(int x, int y);

	//abstracts (for wndproc)
	virtual void OnDisplayChange() = 0;
	virtual void OnDpiChanged(const double& dpi) = 0;

private:
	bool InitWindow(HINSTANCE, LPCWSTR str);
	HWND CreateWindowInternal(const HINSTANCE hInstance, LPCWSTR str);
	UIElement baseElement = nullptr;
	
protected:
	void SetXamlContent(const UIElement& element);
	UIElement GetXamlContent();
	virtual UIElement BuildUIElement();
	DesktopWindowXamlSource _xamlSource;

	HWND hwndParent;
	HWND hwndChild; //xamlhost
	int monitorHeight = 0;
	int monitorWidth = 0;
	int height = 0;
	int width = 0;
	int x = 0;
	int y = 0;
	double effectiveDpi = 1;
};