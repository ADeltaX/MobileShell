#include "pch.h"
#include "NavBarWindow.h"
#include "TopPadWindow.h"
#include "Utils.h"
#include "WindowEnumeration.h"
#include "winrt/Windows.Devices.Haptics.h"
#include "winrt/Windows.UI.Xaml.Controls.h"

using namespace winrt;
using namespace Windows;
using namespace Windows::ApplicationModel::Core;
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
using namespace Windows::UI::Input;
using namespace Windows::UI::Xaml::Input;

void EnumPls(bool);

bool is_navbarhidden = false;
bool is_swiped = false;
static Windows::Devices::Haptics::SimpleHapticsController controller = nullptr;
static Windows::Devices::Haptics::SimpleHapticsControllerFeedback controllerFeedback = nullptr;

FontIcon fBack = nullptr;
FrameworkElement fWindows = nullptr;
FontIcon fSearch = nullptr;

UIElement NavBarWindow::BuildUIElement()
{
	Grid wrp;

	const Grid xaml_container;

	const hstring we(LR"(<Grid x:Name="gridTaskBar"
    xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
	xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml"
	xmlns:mc="http://schemas.openxmlformats.org/markup-compatibility/2006">
    <Grid.Resources>
        <ResourceDictionary>
            <ResourceDictionary.ThemeDictionaries>
                <ResourceDictionary x:Key="Light">
                    <SolidColorBrush x:Key="SysBrush" Color="#000000"/>
                </ResourceDictionary>
                <ResourceDictionary x:Key="Dark">
                    <SolidColorBrush x:Key="SysBrush" Color="#BFBFBF"/>
                </ResourceDictionary>
                <ResourceDictionary x:Key="HighContrast">
                    <SolidColorBrush x:Key="SysBrush" Color="#FFFFFF"/>
                </ResourceDictionary>
            </ResourceDictionary.ThemeDictionaries>
        </ResourceDictionary>
    </Grid.Resources>

    <Grid>
        <Grid.Resources>
            <Style TargetType="FontIcon">
                <Setter Property="Foreground" Value="{ThemeResource SysBrush}" />
                <Setter Property="VerticalAlignment" Value="Center"/>
                <Setter Property="HorizontalAlignment" Value="Center"/>
            </Style>
        </Grid.Resources>

        <Grid.ColumnDefinitions>
            <ColumnDefinition/>
            <ColumnDefinition Width="112"/>
            <ColumnDefinition Width="112"/>
            <ColumnDefinition Width="112"/>
            <ColumnDefinition/>
        </Grid.ColumnDefinitions>

        <Grid x:Name="doubleTapGrid" Grid.ColumnSpan="5" Background="{ThemeResource AppBarBorderThemeBrush}" />

        <Button x:Name="btnBack" Padding="0" Grid.Column="1" Height="48" Width="48" HorizontalAlignment="Center" VerticalAlignment="Center" Style="{StaticResource CommandBarFlyoutEllipsisButtonStyle}">
            <FontIcon x:Name="fBack" Glyph="&#xE72B;" Rotation="0" CenterPoint="10,10,0">
                <FontIcon.RotationTransition>
                    <ScalarTransition />
                </FontIcon.RotationTransition>
            </FontIcon>
        </Button>

        <Button x:Name="btnWindows" Padding="0" Grid.Column="2" Height="48" Width="48" HorizontalAlignment="Center" VerticalAlignment="Center" Style="{StaticResource CommandBarFlyoutEllipsisButtonStyle}" >
			<Viewbox x:Name="fWindows" Height="24" Width="24" Rotation="0" CenterPoint="10,10,0">
				<Canvas Width="87" Height="87">
					<Path Data="M2.2 11.8l33.5-4.6v34.5H0V14.3C0 13 0.9 12 2.2 11.8z">
						<Path.Fill>
							<LinearGradientBrush>
								<LinearGradientBrush.GradientStops>
									<GradientStopCollection>
										<GradientStop Color="#2B75A4" Offset="0"/>
										<GradientStop Color="#2281B6" Offset="0.2487"/>
										<GradientStop Color="#08A2EA" Offset="1"/>
									</GradientStopCollection>
								</LinearGradientBrush.GradientStops>
							</LinearGradientBrush>
						</Path.Fill>
					</Path>
            
					<Path Data="M40 6.5L84.4 0c1.5-0.2 2.9 0.9 2.9 2.5v38.9H40V6.5z">
						<Path.Fill>
							<LinearGradientBrush>
								<LinearGradientBrush.GradientStops>
									<GradientStopCollection>
										<GradientStop Color="#0AC6F3" Offset="0"/>
										<GradientStop Color="#05CCF6" Offset="1"/>
									</GradientStopCollection>
								</LinearGradientBrush.GradientStops>
							</LinearGradientBrush>
						</Path.Fill>
					</Path>
            
					<Path Data="M0 45.3h35.7v34.6L2.2 75.3C0.9 75.1 0 74 0 72.8V45.3z">
						<Path.Fill>
							<LinearGradientBrush>
								<LinearGradientBrush.GradientStops>
									<GradientStopCollection>
										<GradientStop Color="#1B5384" Offset="0"/>
										<GradientStop Color="#134F8D" Offset="1"/>
									</GradientStopCollection>
								</LinearGradientBrush.GradientStops>
							</LinearGradientBrush>
						</Path.Fill>
					</Path>
            
					<Path Data="M40 45.8h47.3v38.5c0 1.5-1.3 2.7-2.9 2.5L40 80.5V45.8z">
						<Path.Fill>
							<LinearGradientBrush>
								<LinearGradientBrush.GradientStops>
									<GradientStopCollection>
										<GradientStop Color="#08A5EC" Offset="0"/>
										<GradientStop Color="#0E9ADD" Offset="0.2175"/>
										<GradientStop Color="#1A84C0" Offset="0.7156"/>
										<GradientStop Color="#1F7CB5" Offset="1"/>
									</GradientStopCollection>
								</LinearGradientBrush.GradientStops>
							</LinearGradientBrush>
						</Path.Fill>
					</Path>
				</Canvas>
			</Viewbox>            
        </Button>

        <Button x:Name="btnSearch" Padding="0" Grid.Column="3" Height="48" Width="48" HorizontalAlignment="Center" VerticalAlignment="Center" Style="{StaticResource CommandBarFlyoutEllipsisButtonStyle}" >
            <FontIcon x:Name="fSearch" Glyph="&#xE11A;" Rotation="0" CenterPoint="10,10,0">
                <FontIcon.RotationTransition>
                    <ScalarTransition />
                </FontIcon.RotationTransition>
            </FontIcon>
        </Button>
    </Grid>
</Grid>)");

	const auto ins = XamlReader::Load(we).as<FrameworkElement>();

	xamlDispatcher = ins.Dispatcher();

	const auto double_tap_grid = ins.FindName(L"doubleTapGrid").as<Grid>();
	const auto double_tap_grid_click = [this](IInspectable const&, DoubleTappedRoutedEventArgs const&)
	{
		//2 = off
		SendMessage(HWND_BROADCAST, WM_SYSCOMMAND, SC_MONITORPOWER, 2);
	};
	double_tap_grid.DoubleTapped(double_tap_grid_click);
	
	const auto btn_back = ins.FindName(L"btnBack").as<Button>();
	const auto btn_back_click = [this](IInspectable const&, RoutedEventArgs const&) 
	{
		this->SendHapticFeedback();
		Utils::ClickBack();
	};
	btn_back.Click(btn_back_click);

	//Until I find a way to get "tablet gestures" disabled
	/*const auto btn_back_holding = [this](IInspectable const&, HoldingRoutedEventArgs const&)
	{
		
		Utils::SendKeyStrokes(VK_LWIN, VK_TAB);
	};
	btn_back.Holding(btn_back_holding);*/

	const auto btn_back_right_tapped = [this](IInspectable const&, RightTappedRoutedEventArgs const&)
	{
		this->SendHapticFeedback();
		Utils::ClickTaskView();
	};
	btn_back.RightTapped(btn_back_right_tapped);

	const auto btn_windows = ins.FindName(L"btnWindows").as<Button>();
	const auto btn_windows_click = [this](IInspectable const&, RoutedEventArgs const&)
	{
		this->SendHapticFeedback();
		Utils::ClickStartMenu();
	};
	btn_windows.Click(btn_windows_click);

	const auto btn_windows_right_tapped = [=](IInspectable const&, RoutedEventArgs const&)
	{
		this->SendHapticFeedback();
		Utils::SendKeyStrokes(VK_LWIN, 0x58); //KEY X
	};
	btn_windows.RightTapped(btn_windows_right_tapped);


	const auto btn_search = ins.FindName(L"btnSearch").as<Button>();
	const auto btn_search_click = [this](IInspectable const&, RoutedEventArgs const&)
	{
		this->SendHapticFeedback();
		Utils::ClickSearch();
	};
	btn_search.Click(btn_search_click);

	fBack = ins.FindName(L"fBack").as<FontIcon>();
	fWindows = ins.FindName(L"fWindows").as<FrameworkElement>();
	fSearch = ins.FindName(L"fSearch").as<FontIcon>();

	ins.ManipulationMode(ManipulationModes::All);

	const auto ins_manipulation_delta = [=](IInspectable const&, ManipulationDeltaRoutedEventArgs const& args)
	{
		if (args.IsInertial() && !is_swiped)
		{
			const auto swiped_distance_x = args.Cumulative().Translation.X;

			if (abs(swiped_distance_x) > 15)
			{
				if (swiped_distance_x > 0)
				{
					EnumPls(false);
				}
				else
				{
					EnumPls(true);
				}
				is_swiped = true;
			}
		}
	};
	ins.ManipulationDelta(ins_manipulation_delta);

	const auto ins_manipulation_completed = [&](IInspectable const&, ManipulationCompletedRoutedEventArgs const&)
	{
		is_swiped = false;
	};
	ins.ManipulationCompleted(ins_manipulation_completed);

	xaml_container.Children().Append(ins);

	xaml_container.CornerRadius(CornerRadius{ 2, 2, 2, 2 });
	xaml_container.UpdateLayout();

	wrp.Children().Append(xaml_container);

	HandleRotation(wrp, xaml_container);

	base = wrp;
	child = xaml_container;

	return wrp;
}

int pos = 0;

void EnumPls(bool left)
{
	auto windooows = EnumerateWindows();
	if (left)
	{
		if (pos > 0)
		{
			pos--;
			
			if (pos > windooows.size())
				SetForegroundWindow(windooows.at(windooows.size() - 1).Hwnd());
			else
				SetForegroundWindow(windooows.at(pos).Hwnd());
		}
	}
	else
	{
		if (pos < windooows.size() - 1)
		{
			pos++;
			SetForegroundWindow(windooows.at(pos).Hwnd());
		}
	}
	
}

void NavBarWindow::HandleRotation(const FrameworkElement& base, const FrameworkElement& child)
{
	const auto orientation = Utils::GetCurrentOrientation();

	if (orientation == DMDO_DEFAULT || orientation == DMDO_180)
	{
		base.Width(std::nan("0"));
		base.Height(std::nan("0"));
		child.Width(std::nan("0"));
		child.RenderTransform(nullptr);
		child.HorizontalAlignment(HorizontalAlignment::Stretch);
	}
	else
	{
		base.Height(monitorHeight / effectiveDpi); //monitor height
		base.Width(navBarHeight);

		RotateTransform trf{};

		if (orientation == DMDO_90)
			trf.Angle(90);
		else
			trf.Angle(270);
		
		trf.CenterX((monitorHeight / effectiveDpi) / 2); //monitor height / 2
		trf.CenterY((monitorHeight / effectiveDpi) / 2);  //navbar height / 2

		child.Width((monitorHeight / effectiveDpi)); //monitor height
		child.RenderTransform(trf);
		child.HorizontalAlignment(HorizontalAlignment::Center);
	}
	
	if (orientation == DMDO_90)
	{
		fSearch.Rotation(-90);
		fWindows.Rotation(-90);
		fBack.Rotation(-90);
	}
	else if (orientation == DMDO_270)
	{
		fSearch.Rotation(90);
		fWindows.Rotation(90);
		fBack.Rotation(90);
	}
	else
	{
		fSearch.Rotation(0);
		fWindows.Rotation(0);
		fBack.Rotation(0);
	}
}

void NavBarWindow::SendHapticFeedback()
{
	//We may want to disable vibration if we don't want it
	if (controller != nullptr && controllerFeedback != nullptr)
	{
		const TimeSpan span(1000000L);
		controller.SendHapticFeedbackForDuration(controllerFeedback, 0.2, span);
	}
}

void NavBarWindow::SetupHaptics()
{
	const auto auth{ Windows::Devices::Haptics::VibrationDevice::RequestAccessAsync().get() };

	if (auth == Windows::Devices::Haptics::VibrationAccessStatus::Allowed)
	{
		const auto device = Windows::Devices::Haptics::VibrationDevice::GetDefaultAsync().get();
		if (device != nullptr)
		{
			controller = device.SimpleHapticsController();
			controllerFeedback = controller.SupportedFeedback().GetAt(0);
		}
	}
}

void NavBarWindow::SetupAppBar(bool istabletmode)
{
	const auto orientation = Utils::GetCurrentOrientation();

	if (istabletmode)
	{
		if (orientation == DMDO_DEFAULT || orientation == DMDO_180)
		{
			if (appbarMessageId != -1u)
				Utils::ABSetPos(hwndParent, width * effectiveDpi, height, effectiveDpi, ABE_BOTTOM);
			else
				appbarMessageId = Utils::RegisterAppBar(hwndParent, width * effectiveDpi, height, effectiveDpi, ABE_BOTTOM);

			Utils::SetWinTaskbarPosition(TSB_BOTTOM);
		}
		else if (orientation == DMDO_90)
		{
			if (appbarMessageId != -1)
				Utils::ABSetPos(hwndParent, width, height, effectiveDpi, ABE_LEFT);
			else
				appbarMessageId = Utils::RegisterAppBar(hwndParent, width, height, effectiveDpi, ABE_LEFT);

			Utils::SetWinTaskbarPosition(TSB_LEFT);
		}
		else if (orientation == DMDO_270)
		{
			if (appbarMessageId != -1)
				Utils::ABSetPos(hwndParent, width, height, effectiveDpi, ABE_RIGHT);
			else
				appbarMessageId = Utils::RegisterAppBar(hwndParent, width, height, effectiveDpi, ABE_RIGHT);

			Utils::SetWinTaskbarPosition(TSB_RIGHT);
		}

		xamlDispatcher.RunAsync(winrt::Windows::UI::Core::CoreDispatcherPriority::Normal, Windows::UI::Core::DispatchedHandler([=]()
			{
				HandleRotation(base, child);
			}));
	}
	else
	{
		appbarMessageId = Utils::UnregisterAppBar(hwndParent);
	}
}
