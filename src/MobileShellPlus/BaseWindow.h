#pragma once
#include "pch.h"

using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Hosting;

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