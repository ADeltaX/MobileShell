#pragma once

#include "pch.h"
#include "BaseWindow.h"
#include "Utils.h"

class TopPadWindow : public BaseWindow
{
	//const
	int const navBarHeight = 48;
	UINT appbarMessageId = -1;
	FrameworkElement base = nullptr;
	FrameworkElement child = nullptr;

public:
	explicit TopPadWindow(const HINSTANCE hInstance) : BaseWindow(hInstance, L"Padding")
	{
		Configure();
	}

	UIElement BuildUIElement() override;
	void SetupAppBar();
	void ShowTimed()
	{
		Show();
		SetupAppBar();

		Sleep(4000);
		
	}

private:
	void OnDisplayChange() override
	{
		Configure();

		//TODO: IF tablet mode huh
		SetupAppBar();
	}

	void OnDpiChanged(const double& dpi) override
	{
		Configure();

		//TODO: IF tablet mode huh
		SetupAppBar();
	}

	void Configure()
	{
		Utils::GetMonitorSizeAndDpiFromWindow(this->hwndParent,
			&this->effectiveDpi,
			&this->monitorHeight,
			&this->monitorWidth);

		const auto orientation = Utils::GetCurrentOrientation();

		if (orientation == DMDO_DEFAULT || orientation == DMDO_180)
		{
			SetSize(monitorWidth, std::ceil(monitorHeight / 2.25), effectiveDpi);
			SetPosition(0, 0);
		}
	}
};