#pragma once
#include "pch.h"
#include "BaseWindow.h"
#include "Utils.h"

class TrashWindow :
	public BaseWindow
{
	//const
	//int const notificationWindowMaxWidth = 384;

public:
	explicit TrashWindow(HINSTANCE hInstance) : BaseWindow(hInstance, L"TrashWindow", ZBID_DESKTOP)
	{
		SetDpi();
	}

	UIElement BuildUIElement() override;

private:
	void OnDisplayChange() override
	{
		const auto orientation = Utils::GetCurrentOrientation();

		if (orientation == DMDO_DEFAULT || orientation == DMDO_270)
		{

		}

		SetDpi();
	}

	void OnDpiChanged(const double& dpi) override
	{
		SetDpi();
	}

	void SetDpi()
	{
		Utils::GetMonitorSizeAndDpiFromWindow(this->hwndParent,
			&this->effectiveDpi,
			&this->monitorHeight,
			&this->monitorWidth);

		SetSize(400, monitorHeight - ((48L + 24) * effectiveDpi), effectiveDpi);
		SetPosition(0, 24 * effectiveDpi);
	}
};