#pragma once
#include "pch.h"
#include "BaseWindow.h"
#include "Utils.h"
#include "Wnf.h"

#pragma once
class NavBarWindow : public BaseWindow
{
	//const
	int const navBarHeight = 48;
	UINT appbarMessageId = -1;
	FrameworkElement base = nullptr;
	FrameworkElement child = nullptr;

	winrt::Windows::UI::Core::CoreDispatcher xamlDispatcher = nullptr;

public:
	explicit NavBarWindow(const HINSTANCE hInstance) : BaseWindow(hInstance)
	{
		Configure();
	}

	UIElement BuildUIElement() override;
	void HandleRotation(const FrameworkElement& base, const FrameworkElement& child);
	void SendHapticFeedback();
	void SetupHaptics();
	void SetupAppBar(bool istabletmode);

private:
	void OnDisplayChange() override
	{
		Configure();

		if (Wnf::IsTabletMode())
			SetupAppBar(true);
	}
	
	void OnDpiChanged(const double& dpi) override
	{
		Configure();

		if (Wnf::IsTabletMode())
			SetupAppBar(true);
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
			SetSize(monitorWidth, std::ceil(navBarHeight * effectiveDpi), effectiveDpi);
			SetPosition(0, std::ceil(monitorHeight - (navBarHeight * effectiveDpi)));
		}
		else if (orientation == DMDO_90)
		{
			SetSize(std::ceil(navBarHeight * effectiveDpi), monitorHeight, effectiveDpi);
			SetPosition(0, 0);
		}
		else if (orientation == DMDO_270)
		{
			SetSize(std::ceil(navBarHeight * effectiveDpi), monitorHeight, effectiveDpi);
			SetPosition(monitorWidth - (navBarHeight * effectiveDpi), 0);
		}
	}
};

