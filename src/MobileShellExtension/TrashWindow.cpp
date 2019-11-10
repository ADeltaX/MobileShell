#include "pch.h"
#include "TrashWindow.h"

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


UIElement TrashWindow::BuildUIElement()
{
	Grid xamlContainer;

	Border compBeh;

	AcrylicBrush aBrush;
	aBrush.BackgroundSource(AcrylicBackgroundSource::HostBackdrop);
	aBrush.TintOpacity(0.5);
	aBrush.TintLuminosityOpacity(0.5);

	compBeh.Background(aBrush);

	/*auto _compositor = ElementCompositionPreview::GetElementVisual(compBeh).Compositor();
	auto _hostSprite = _compositor.CreateSpriteVisual();
	_hostSprite.Size(float2(width, height));
	ElementCompositionPreview::SetElementChildVisual(compBeh, _hostSprite);
	_hostSprite.Brush(_compositor.CreateHostBackdropBrush()); */

	xamlContainer.Children().Append(compBeh);

	xamlContainer.CornerRadius(CornerRadius{ 2, 2, 2, 2 });
	xamlContainer.UpdateLayout();

	return xamlContainer;
}