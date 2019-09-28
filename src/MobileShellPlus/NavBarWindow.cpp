﻿#include "pch.h"
#include "NavBarWindow.h"
#include "winrt/Windows.Devices.Haptics.h"
#include "Utils.h"
#include "winrt/Windows.UI.Xaml.Controls.h"
#include "WindowEnumeration.h"

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

bool is_swiped = false;
static Windows::Devices::Haptics::SimpleHapticsController controller = nullptr;
static Windows::Devices::Haptics::SimpleHapticsControllerFeedback controllerFeedback = nullptr;

FontIcon fBack = nullptr;
FontIcon fWindows = nullptr;
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

        <Button x:Name="btnWindows"  Padding="0" Grid.Column="2" Height="48" Width="48" HorizontalAlignment="Center" VerticalAlignment="Center" Style="{StaticResource CommandBarFlyoutEllipsisButtonStyle}" >
            <FontIcon x:Name="fWindows" Glyph="&#xE8B3;" Rotation="0" CenterPoint="10,10,0">
                <FontIcon.RotationTransition>
                    <ScalarTransition />
                </FontIcon.RotationTransition>
            </FontIcon>
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
		Utils::SendKeyStrokes(VK_LWIN, VK_BACK);
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
		Utils::OpenTaskView();
	};
	btn_back.RightTapped(btn_back_right_tapped);

	const auto btn_back_double_tapped = [this](IInspectable const&, DoubleTappedRoutedEventArgs const&)
	{
		
	};
	btn_back.DoubleTapped(btn_back_double_tapped);


	const auto btn_windows = ins.FindName(L"btnWindows").as<Button>();
	const auto btn_windows_click = [this](IInspectable const&, RoutedEventArgs const&)
	{
		this->SendHapticFeedback();
		Utils::SendKeyStroke(VK_LWIN);
	};
	btn_windows.Click(btn_windows_click);

	const auto btn_windows_right_tapped = [this](IInspectable const&, RoutedEventArgs const&)
	{
		this->SendHapticFeedback();
		Utils::SendKeyStrokes(VK_LWIN, 0x58); //KEY X
	};
	btn_windows.RightTapped(btn_windows_right_tapped);


	const auto btn_search = ins.FindName(L"btnSearch").as<Button>();
	const auto btn_search_click = [this](IInspectable const&, RoutedEventArgs const&)
	{
		this->SendHapticFeedback();
		Utils::SendKeyStrokes(VK_LWIN, 0x51); //KEY S
	};
	btn_search.Click(btn_search_click);

	fBack = ins.FindName(L"fBack").as<FontIcon>();
	fWindows = ins.FindName(L"fWindows").as<FontIcon>();
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

void NavBarWindow::SetupAppBar()
{
	const auto orientation = Utils::GetCurrentOrientation();

	if (orientation == DMDO_DEFAULT || orientation == DMDO_180)
	{
		appbarMessageId = Utils::RegisterAppBar(hwndParent, width * effectiveDpi, height, effectiveDpi, ABE_BOTTOM);
	}
	else if (orientation == DMDO_90)
	{
		appbarMessageId = Utils::RegisterAppBar(hwndParent, width, height, effectiveDpi, ABE_LEFT);
	}
	else if (orientation == DMDO_270)
	{
		appbarMessageId = Utils::RegisterAppBar(hwndParent, width, height, effectiveDpi, ABE_RIGHT);
	}

	HandleRotation(base, child);
}
