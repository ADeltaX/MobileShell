#pragma once
#include "pch.h"
#include "BaseWindow.h"
#include "Utils.h"
#include "Wnf.h"
#include "winrt/Windows.ApplicationModel.Calls.h"

class StatusBarWindow : public BaseWindow
{
	//const
	int const statusBarHeight = 24;
	UINT appbarMessageId = -1;
	FrameworkElement base = nullptr;
	FrameworkElement child = nullptr;

	winrt::Windows::UI::Core::CoreDispatcher xamlDispatcher = nullptr;

public:
	explicit StatusBarWindow(const HINSTANCE hInstance) : BaseWindow(hInstance, L"StatusBar")
	{
		Configure();
	}

	UIElement BuildUIElement() override;
	void HandleRotation(const FrameworkElement& base, const FrameworkElement& child);
	void SetWiFiSignal(unsigned char value);
	void SetVPN(bool enabled);
	void SetLocation(bool inUse);
	void SetWiFi(bool enabled);
	void SetBluetooth(bool enabled);
	void SetAirplaneMode(bool enabled);
	void SetNotifications(int num);
	void SetFocusAssist(FocusAssistStatus status);
	void UpdateBatteryPercentage();
	void UpdateSim1(winrt::Windows::ApplicationModel::Calls::PhoneLine phone_line);
	void UpdateSim2(winrt::Windows::ApplicationModel::Calls::PhoneLine phone_line);
	void SetDualSim();
	void SetSim1Signal(unsigned char value);
	void SetSim2Signal(unsigned char value);
	void SetupAppBar(bool istabletmode);
	winrt::hstring GetBatteryStatusGlyph(int current_battery, bool charging, bool energy_saving_mode);

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
			SetSize(monitorWidth, std::ceil(statusBarHeight * effectiveDpi), effectiveDpi);
			SetPosition(0, 0);
		}
		else if (orientation == DMDO_90)
		{
			SetSize(std::ceil(statusBarHeight * 2.0 * effectiveDpi), monitorHeight, effectiveDpi);
			SetPosition(monitorWidth - (statusBarHeight * 2.0 * effectiveDpi), 0);
		}
		else if (orientation == DMDO_270)
		{
			SetSize(std::ceil(statusBarHeight * 2.0 * effectiveDpi), monitorHeight, effectiveDpi);
			SetPosition(0, 0);
		}
	}
};


