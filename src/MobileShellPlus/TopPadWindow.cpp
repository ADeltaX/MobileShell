#include "pch.h"
#include "TopPadWindow.h"

UIElement TopPadWindow::BuildUIElement()
{
	Controls::Grid grid{};
	Media::AcrylicBrush bruh;

	bruh.BackgroundSource(Media::AcrylicBackgroundSource::HostBackdrop);
	
	grid.Background(bruh);
	
	return grid;
}

void TopPadWindow::SetupAppBar()
{
	const auto orientation = Utils::GetCurrentOrientation();

	if (orientation == DMDO_DEFAULT || orientation == DMDO_180)
	{
		if (appbarMessageId != -1u)
			Utils::ABSetPos(hwndParent, width * effectiveDpi, height, effectiveDpi, ABE_TOP);
		else
			appbarMessageId = Utils::RegisterAppBar(hwndParent, width * effectiveDpi, height, effectiveDpi, ABE_TOP);
	}
}
