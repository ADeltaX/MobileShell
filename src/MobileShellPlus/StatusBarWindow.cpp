#include "pch.h"
#include "StatusBarWindow.h"
#include "Utils.h"
#include "Wnf.h"
#include "winrt/Windows.System.Power.h"
#include <iomanip>
#include <ctime>
#include <sstream>

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Numerics;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Markup;
using namespace Windows::UI::Xaml::Hosting;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI;
using namespace Windows::UI::Composition;
using namespace Windows::System::Power;

TextBlock clockTextBox = nullptr;
TextBlock batteryPercentage = nullptr;
FontIcon batteryGlyph = nullptr;
Grid notificationGrid = nullptr;
Grid focusGrid = nullptr;
Grid airplaneGrid = nullptr;
Grid locationGrid = nullptr;
Grid bluetoothGrid = nullptr;
Grid wifiGrid = nullptr;
FontIcon wifiGlyph = nullptr;
Grid vpnGrid = nullptr;
Grid roamingGrid = nullptr;

Grid sim1Grid = nullptr;
Grid sim2Grid = nullptr;
FontIcon sim1Glyph = nullptr;
FontIcon sim2Glyph = nullptr;
FontIcon sim1Dual = nullptr;
FontIcon sim2Dual = nullptr;
FontIcon sim1SignalStatus = nullptr;
FontIcon sim2SignalStatus = nullptr;
FontIcon sim1Signal = nullptr;
FontIcon sim2Signal = nullptr;
FontIcon sim1Back = nullptr;
FontIcon sim2Back = nullptr;

bool isDualSim = false;
bool isAirplaneMode = false;

winrt::Windows::UI::Core::CoreDispatcher xamlDispatcher(nullptr);

constexpr auto BATTERY_NOT_PRESENT_GLYPH = L"\uEC02";


UIElement StatusBarWindow::BuildUIElement()
{
	Grid wrp;
	
	Grid xaml_container;

	const hstring xaml(LR"(<Grid
    xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
    xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml"
    xmlns:d="http://schemas.microsoft.com/expression/blend/2008">
    <Grid.Resources>
        <ResourceDictionary>
            <ResourceDictionary.ThemeDictionaries>
                <ResourceDictionary x:Key="Light">
                    <SolidColorBrush x:Key="SysBrush" Color="#666666"/>
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

    <Grid x:Name="gridStatusBar" Height="24" Padding="9,0" VerticalAlignment="Center">
        <Grid.Resources>
            <Style TargetType="FontIcon">
                <Setter Property="Foreground" Value="{ThemeResource SysBrush}" />
                <Setter Property="FontSize" Value="16"/>
                <Setter Property="VerticalAlignment" Value="Center"/>
                <Setter Property="HorizontalAlignment" Value="Center"/>
            </Style>
            <Style TargetType="TextBlock">
                <Setter Property="Foreground" Value="{ThemeResource SysBrush}" />
            </Style>
        </Grid.Resources>
        <Grid.ColumnDefinitions>
            <ColumnDefinition Width="Auto"/>
            <ColumnDefinition/>
            <ColumnDefinition Width="Auto"/>
        </Grid.ColumnDefinitions>
        <StackPanel Orientation="Horizontal">

            <Grid x:Name="airplaneGrid" Width="16" Margin="3,0" Visibility="Collapsed" Rotation="0" CenterPoint="8,12,0">
                <Grid.RotationTransition>
                    <ScalarTransition />
                </Grid.RotationTransition>
                <FontIcon x:Name="airplaneGlyph" Glyph="&#xEC40;" />
            </Grid>

            <Grid x:Name="sim1Grid" Width="16" Margin="3,0" Visibility="Collapsed" Rotation="0" CenterPoint="8,12,0">
                <Grid.RotationTransition>
                    <ScalarTransition />
                </Grid.RotationTransition>
                <FontIcon x:Name="sim1SignalStatus" Glyph="&#xE871;" Visibility="Collapsed" />
                <FontIcon x:Name="sim1Signal" Glyph="&#xE86C;" Visibility="Collapsed" />
                <FontIcon x:Name="sim1Back" Opacity="0.5" Glyph="&#xEC3B;" Visibility="Collapsed" />
                <FontIcon x:Name="sim1Dual" Glyph="&#xE884;" Visibility="Collapsed" />
                <FontIcon x:Name="sim1Glyph" Glyph="&#xE876;" Visibility="Collapsed" />
            </Grid>

            <Grid x:Name="sim2Grid" Width="16" Margin="3,0" Visibility="Collapsed" Rotation="0" CenterPoint="8,12,0">
                <Grid.RotationTransition>
                    <ScalarTransition />
                </Grid.RotationTransition>
                <FontIcon x:Name="sim2SignalStatus" Glyph="&#xE871;" Visibility="Collapsed" />
                <FontIcon x:Name="sim2Signal" Glyph="&#xE86C;" Visibility="Collapsed" />
                <FontIcon x:Name="sim2Back" Opacity="0.5" Glyph="&#xEC3B;" Visibility="Collapsed" />
                <FontIcon x:Name="sim2Dual" Glyph="&#xE882;" Visibility="Collapsed" />
                <FontIcon x:Name="sim2Glyph" Glyph="&#xE876;" Visibility="Visible" />
            </Grid>

            <Grid x:Name="roamingGrid" Visibility="Collapsed" Rotation="0" CenterPoint="8,12,0">
                <Grid.RotationTransition>
                    <ScalarTransition />
                </Grid.RotationTransition>
                <FontIcon x:Name="roamingGlyph" Glyph="&#xE878;" Margin="0,0,0,2" />
            </Grid>

            <Grid x:Name="wifiGrid" Margin="3,0" Visibility="Collapsed" Rotation="0" CenterPoint="8,12,0">
                <Grid.RotationTransition>
                    <ScalarTransition />
                </Grid.RotationTransition>
                <FontIcon Opacity="0.5" Glyph="&#xEC3F;"/>
                <FontIcon x:Name="wifiGlyph" Glyph="&#xD;"/>
            </Grid>

            <Grid x:Name="tetheringGrid" Margin="3,0" Visibility="Collapsed" Rotation="0" CenterPoint="8,12,0">
                <Grid.RotationTransition>
                    <ScalarTransition />
                </Grid.RotationTransition>
                <FontIcon x:Name="tetheringGlyph"  Glyph="&#xEC44;"/>
            </Grid>

            <Grid x:Name="ethernetGrid" Margin="3,0" Visibility="Collapsed" Rotation="0" CenterPoint="8,12,0">
                <Grid.RotationTransition>
                    <ScalarTransition />
                </Grid.RotationTransition>
                <FontIcon x:Name="ethernetGlyph"  Glyph="&#xE839;"/>
            </Grid>

            <Grid x:Name="vpnGrid" Margin="3,0" Visibility="Collapsed" Rotation="0" CenterPoint="8,12,0">
                <Grid.RotationTransition>
                    <ScalarTransition />
                </Grid.RotationTransition>
                <FontIcon x:Name="vpnGlyph"  Glyph="&#xE705;"/>
            </Grid>

            <Grid x:Name="vibrateRingGrid" Margin="3,0" Visibility="Collapsed" Rotation="0" CenterPoint="8,12,0">
                <Grid.RotationTransition>
                    <ScalarTransition />
                </Grid.RotationTransition>
                <FontIcon x:Name="vibrateRingGlyph"  Glyph="&#xE877;" />
            </Grid>

            <Grid x:Name="focusGrid" Margin="3,0" Visibility="Collapsed" Rotation="0" CenterPoint="8,12,0">
                <Grid.RotationTransition>
                    <ScalarTransition />
                </Grid.RotationTransition>
                <FontIcon x:Name="focusGlyph" Glyph="&#xEC46;" />
            </Grid>

            <Grid x:Name="locationGrid" Margin="3,0" Visibility="Collapsed" Rotation="0" CenterPoint="8,12,0">
                <Grid.RotationTransition>
                    <ScalarTransition />
                </Grid.RotationTransition>
                <FontIcon x:Name="locationGlyph"  Glyph="&#xEC43;" />
            </Grid>

            <Grid x:Name="bluetoothGrid" Margin="3,0" Visibility="Collapsed" Rotation="0" CenterPoint="8,12,0">
                <Grid.RotationTransition>
                    <ScalarTransition />
                </Grid.RotationTransition>
                <FontIcon x:Name="bluetoothGlyph" Opacity="0.5" Glyph="&#xEC41;" />
            </Grid>

            <Grid x:Name="notificationGrid" Margin="3,0" Visibility="Collapsed" Rotation="0" CenterPoint="8,12,0">
                <Grid.RotationTransition>
                    <ScalarTransition />
                </Grid.RotationTransition>
                <FontIcon x:Name="notificationGlyph" Glyph="&#xEC42;"/>
            </Grid>

        </StackPanel>

        <StackPanel Orientation="Horizontal" Grid.Column="2">
            <TextBlock x:Name="batteryPercentage" FontSize="10" Text="5%" VerticalAlignment="Center" Visibility="Visible" TextAlignment="Center" Margin="0,0,0,2" Rotation="0" CenterPoint="9.5,6.5,0" />

            <FontIcon x:Name="batteryGlyph" Glyph="&#xEC02;" Margin="4,0" Rotation="0" CenterPoint="12,8,0" />

            <TextBlock Margin="3,0,3,2" x:Name="clockTextBox" FontSize="13" Text="05:05" VerticalAlignment="Center" TextAlignment="Center" Rotation="0" CenterPoint="14,8.5,0" />
        </StackPanel>

        <Grid x:Name="Action Center" Grid.ColumnSpan="3" HorizontalAlignment="Stretch" />
    </Grid>
</Grid>)");

	const auto ins = XamlReader::Load(xaml).as<FrameworkElement>();

	xamlDispatcher = ins.Dispatcher();

	clockTextBox = ins.FindName(L"clockTextBox").as<TextBlock>();

	//Performance issue, need to investigate
	TimeSpan dur(20000000L);
	
	DispatcherTimer dispClock;
	dispClock.Interval(dur);
	dispClock.Tick([=](IInspectable const&, IInspectable const&)
	{
		auto t = std::time(nullptr);
		auto tm = *std::localtime(&t);

		std::ostringstream oss;
		oss << std::put_time(&tm, "%H:%M");
		auto str = oss.str();

		//now fake it - no failures assured /s
		const auto wstr = std::wstring(str.begin(), str.end());
		auto result = wstr.c_str();
		
		clockTextBox.Text(hstring(wstr));
	});

	dispClock.Start();
	
#pragma region Battery

	batteryPercentage = ins.FindName(L"batteryPercentage").as<TextBlock>();
	batteryGlyph = ins.FindName(L"batteryGlyph").as<FontIcon>();

#pragma endregion

#pragma region Notifications

	notificationGrid = ins.FindName(L"notificationGrid").as<Grid>();

#pragma endregion

#pragma region Focus Assist

	focusGrid = ins.FindName(L"focusGrid").as<Grid>();

#pragma endregion

#pragma region Airplane mode

	airplaneGrid = ins.FindName(L"airplaneGrid").as<Grid>();

#pragma endregion

#pragma region Location

	locationGrid = ins.FindName(L"locationGrid").as<Grid>();

#pragma endregion

#pragma region Bluetooth

	bluetoothGrid = ins.FindName(L"bluetoothGrid").as<Grid>();

#pragma endregion

#pragma region WiFi

	wifiGrid = ins.FindName(L"wifiGrid").as<Grid>();
	wifiGlyph = ins.FindName(L"wifiGlyph").as<FontIcon>();

#pragma endregion

#pragma region VPN

	vpnGrid = ins.FindName(L"vpnGrid").as<Grid>();

#pragma endregion

#pragma region SIM

	sim1Grid = ins.FindName(L"sim1Grid").as<Grid>();
	sim2Grid = ins.FindName(L"sim2Grid").as<Grid>();
	sim1Glyph = ins.FindName(L"sim1Glyph").as<FontIcon>();
	sim2Glyph = ins.FindName(L"sim2Glyph").as<FontIcon>();
	sim1Dual = ins.FindName(L"sim1Dual").as<FontIcon>();
	sim2Dual = ins.FindName(L"sim2Dual").as<FontIcon>();
	sim1SignalStatus = ins.FindName(L"sim1SignalStatus").as<FontIcon>();
	sim2SignalStatus = ins.FindName(L"sim2SignalStatus").as<FontIcon>();
	sim1Signal = ins.FindName(L"sim1Signal").as<FontIcon>();
	sim2Signal = ins.FindName(L"sim2Signal").as<FontIcon>();
	sim1Back = ins.FindName(L"sim1Back").as<FontIcon>();
	sim2Back = ins.FindName(L"sim2Back").as<FontIcon>();
	
#pragma endregion

#pragma region Roaming

	roamingGrid = ins.FindName(L"roamingGrid").as<Grid>();

#pragma endregion

	
	xaml_container.Children().Append(ins);

	xaml_container.CornerRadius(CornerRadius{ 2, 2, 2, 2 });
	xaml_container.UpdateLayout();

	wrp.Children().Append(xaml_container);

	HandleRotation(wrp, xaml_container);

	base = wrp;
	child = xaml_container;

	return wrp;
}

void StatusBarWindow::HandleRotation(const FrameworkElement& base, const FrameworkElement& child)
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
		base.Width(statusBarHeight * 2);

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

	auto OoOf = batteryPercentage.ActualHeight();
	auto OoOf2 = batteryPercentage.ActualWidth();
	
	if (orientation == DMDO_90)
	{
		sim1Grid.Rotation(-90);
		sim2Grid.Rotation(-90);
		notificationGrid.Rotation(-90);
		focusGrid.Rotation(-90);
		airplaneGrid.Rotation(-90);
		locationGrid.Rotation(-90);
		bluetoothGrid.Rotation(-90);
		wifiGrid.Rotation(-90);
		vpnGrid.Rotation(-90);
		roamingGrid.Rotation(-90);

		clockTextBox.Rotation(-90);
		batteryGlyph.Rotation(-90);
		batteryPercentage.Rotation(-90);
	}
	else if (orientation == DMDO_270)
	{
		sim1Grid.Rotation(90);
		sim2Grid.Rotation(90);
		notificationGrid.Rotation(90);
		focusGrid.Rotation(90);
		airplaneGrid.Rotation(90);
		locationGrid.Rotation(90);
		bluetoothGrid.Rotation(90);
		wifiGrid.Rotation(90);
		vpnGrid.Rotation(90);
		roamingGrid.Rotation(90);

		clockTextBox.Rotation(90);
		batteryGlyph.Rotation(90);
		batteryPercentage.Rotation(90);
	}
	else
	{
		sim1Grid.Rotation(0);
		sim2Grid.Rotation(0);
		notificationGrid.Rotation(0);
		focusGrid.Rotation(0);
		airplaneGrid.Rotation(0);
		locationGrid.Rotation(0);
		bluetoothGrid.Rotation(0);
		wifiGrid.Rotation(0);
		vpnGrid.Rotation(0);
		roamingGrid.Rotation(0);

		clockTextBox.Rotation(0);
		batteryGlyph.Rotation(0);
		batteryPercentage.Rotation(0);
	}
}

void StatusBarWindow::SetWiFiSignal(unsigned char value)
{
	xamlDispatcher.RunAsync(winrt::Windows::UI::Core::CoreDispatcherPriority::Normal, Windows::UI::Core::DispatchedHandler([=]()
		{
			switch (value)
			{
			case 1:
				wifiGlyph.Glyph(L"\uEC3C");
				break;
			case 2:
				wifiGlyph.Glyph(L"\uEC3D");
				break;
			case 3:
				wifiGlyph.Glyph(L"\uEC3E");
				break;
			case 4:
			case 5:
				wifiGlyph.Glyph(L"\uEC3F");
				break;
			default:
				//0
				wifiGlyph.Glyph(L"\u000D");
				break;
			}
		}));
}

void StatusBarWindow::SetVPN(bool enabled)
{
	xamlDispatcher.RunAsync(winrt::Windows::UI::Core::CoreDispatcherPriority::Normal, Windows::UI::Core::DispatchedHandler([=]()
		{
			if (enabled)
			{
				vpnGrid.Visibility(Visibility::Visible);
			}
			else
			{
				vpnGrid.Visibility(Visibility::Collapsed);
			}
		}));
}

void StatusBarWindow::SetLocation(bool inUse)
{
	xamlDispatcher.RunAsync(winrt::Windows::UI::Core::CoreDispatcherPriority::Normal, Windows::UI::Core::DispatchedHandler([=]()
		{
			if (inUse)
			{
				locationGrid.Visibility(Visibility::Visible);
			}
			else
			{
				locationGrid.Visibility(Visibility::Collapsed);
			}
		}));
}

void StatusBarWindow::SetWiFi(bool enabled)
{
	xamlDispatcher.RunAsync(winrt::Windows::UI::Core::CoreDispatcherPriority::Normal, Windows::UI::Core::DispatchedHandler([=]()
		{
			if (enabled)
			{
				wifiGrid.Visibility(Visibility::Visible);
			}
			else
			{
				wifiGrid.Visibility(Visibility::Collapsed);
			}
		}));
}

void StatusBarWindow::SetBluetooth(bool enabled)
{
	xamlDispatcher.RunAsync(winrt::Windows::UI::Core::CoreDispatcherPriority::Normal, Windows::UI::Core::DispatchedHandler([=]()
		{
			if (enabled)
			{
				bluetoothGrid.Visibility(Visibility::Visible);
			}
			else
			{
				bluetoothGrid.Visibility(Visibility::Collapsed);
			}
		}));
}

void StatusBarWindow::SetAirplaneMode(bool enabled)
{
	isAirplaneMode = enabled;
	
	xamlDispatcher.RunAsync(winrt::Windows::UI::Core::CoreDispatcherPriority::Normal, Windows::UI::Core::DispatchedHandler([=]()
		{
			if (enabled)
			{
				airplaneGrid.Visibility(Visibility::Visible);
				sim1Grid.Visibility(Visibility::Collapsed);
				sim2Grid.Visibility(Visibility::Collapsed);
			}
			else
			{
				airplaneGrid.Visibility(Visibility::Collapsed);

				sim1Grid.Visibility(Visibility::Visible);
				
				if (isDualSim)
					sim2Grid.Visibility(Visibility::Visible);
			}
		}));
}

void StatusBarWindow::SetNotifications(int num)
{
	xamlDispatcher.RunAsync(winrt::Windows::UI::Core::CoreDispatcherPriority::Normal, Windows::UI::Core::DispatchedHandler([=]()
		{
			if (num > 0)
				notificationGrid.Visibility(Visibility::Visible);
			else
				notificationGrid.Visibility(Visibility::Collapsed);
		}));
}

void StatusBarWindow::SetFocusAssist(FocusAssistStatus status)
{
	xamlDispatcher.RunAsync(winrt::Windows::UI::Core::CoreDispatcherPriority::Normal, Windows::UI::Core::DispatchedHandler([=]()
		{
			if (status != FocusAssistStatus::Off)
				focusGrid.Visibility(Visibility::Visible);
			else
				focusGrid.Visibility(Visibility::Collapsed);
		}));
}

void StatusBarWindow::UpdateBatteryPercentage()
{
	xamlDispatcher.RunAsync(winrt::Windows::UI::Core::CoreDispatcherPriority::Normal, Windows::UI::Core::DispatchedHandler([this]()
		{
			switch (PowerManager::BatteryStatus())
			{
			case BatteryStatus::NotPresent:
				//batteryGlyph.Glyph(BATTERY_NOT_PRESENT_GLYPH);
				batteryGlyph.Visibility(Visibility::Collapsed);
				batteryPercentage.Visibility(Visibility::Collapsed);
				break;

			case BatteryStatus::Charging:
				batteryPercentage.Visibility(Visibility::Visible);
				batteryPercentage.Text(to_hstring(PowerManager::RemainingChargePercent()) + L"%");
				batteryGlyph.Glyph(GetBatteryStatusGlyph(PowerManager::RemainingChargePercent(), true, false));
				break;

			case BatteryStatus::Discharging:
				batteryPercentage.Visibility(Visibility::Visible);
				batteryPercentage.Text(to_hstring(PowerManager::RemainingChargePercent()) + L"%");

				if (PowerManager::EnergySaverStatus() == EnergySaverStatus::On)
					batteryGlyph.Glyph(GetBatteryStatusGlyph(PowerManager::RemainingChargePercent(), false, true));
				else
					batteryGlyph.Glyph(GetBatteryStatusGlyph(PowerManager::RemainingChargePercent(), false, false));
				break;

			case BatteryStatus::Idle:
				batteryPercentage.Visibility(Visibility::Visible);
				batteryPercentage.Text(to_hstring(PowerManager::RemainingChargePercent()) + L"%");
				batteryGlyph.Glyph(GetBatteryStatusGlyph(PowerManager::RemainingChargePercent(), true, false));
				break;
			}
		}));
}

hstring StatusBarWindow::GetBatteryStatusGlyph(const int current_battery, const bool charging, const bool energy_saving_mode)
{
	if (current_battery <= 5)
		return charging ? L"\uEBAB" : (energy_saving_mode ? L"\uEBB6" : L"\uEBA0");
	if (current_battery > 5 && current_battery <= 10)
		return charging ? L"\uEBAC" : (energy_saving_mode ? L"\uEBB7" : L"\uEBA1");
	if (current_battery > 10 && current_battery <= 20)
		return charging ? L"\uEBAD" : (energy_saving_mode ? L"\uEBB8" : L"\uEBA2");
	if (current_battery > 20 && current_battery <= 30)
		return charging ? L"\uEBAE" : (energy_saving_mode ? L"\uEBB9" : L"\uEBA3");
	if (current_battery > 30 && current_battery <= 40)
		return charging ? L"\uEBAF" : (energy_saving_mode ? L"\uEBBA" : L"\uEBA4");
	if (current_battery > 40 && current_battery <= 50)
		return charging ? L"\uEBB0" : (energy_saving_mode ? L"\uEBBB" : L"\uEBA5");
	if (current_battery > 50 && current_battery <= 60)
		return charging ? L"\uEBB1" : (energy_saving_mode ? L"\uEBBC" : L"\uEBA6");
	if (current_battery > 60 && current_battery <= 70)
		return charging ? L"\uEBB2" : (energy_saving_mode ? L"\uEBBD" : L"\uEBA7");
	if (current_battery > 70 && current_battery <= 80)
		return charging ? L"\uEBB3" : (energy_saving_mode ? L"\uEBBE" : L"\uEBA8");
	if (current_battery > 80 && current_battery < 95)
		return charging ? L"\uEBB4" : (energy_saving_mode ? L"\uEBBF" : L"\uEBA9");

	//>= 95
	return charging ? L"\uEBB5" : (energy_saving_mode ? L"\uEBC0" : L"\uEBAA");
}

void StatusBarWindow::SetDualSim()
{
	isDualSim = true;

	xamlDispatcher.RunAsync(winrt::Windows::UI::Core::CoreDispatcherPriority::Normal, Windows::UI::Core::DispatchedHandler([=]()
		{
			sim1Dual.Visibility(Visibility::Visible);
			sim2Dual.Visibility(Visibility::Visible);

			if (!isAirplaneMode)
			{
				sim2Grid.Visibility(Visibility::Visible);
			}
		}));
}

void StatusBarWindow::SetSim1Signal(unsigned char value)
{
	xamlDispatcher.RunAsync(winrt::Windows::UI::Core::CoreDispatcherPriority::Normal, Windows::UI::Core::DispatchedHandler([=]()
		{
			switch (value)
			{
			case 1:
				sim1Signal.Glyph(L"\uEC37");
				break;
			case 2:
				sim1Signal.Glyph(L"\uEC38");
				break;
			case 3:
				sim1Signal.Glyph(L"\uEC39");
				break;
			case 4:
				sim1Signal.Glyph(L"\uEC3A");
				break;
			case 5:
				sim1Signal.Glyph(L"\uEC3B");
				break;
			case 0:
			default:
				sim1Signal.Glyph(L"\u000D");
				break;
			}
		}));
}

void StatusBarWindow::SetSim2Signal(unsigned char value)
{
	xamlDispatcher.RunAsync(winrt::Windows::UI::Core::CoreDispatcherPriority::Normal, Windows::UI::Core::DispatchedHandler([=]()
		{
			switch (value)
			{
			case 1:
				sim2Signal.Glyph(L"\uEC37");
				break;
			case 2:
				sim2Signal.Glyph(L"\uEC38");
				break;
			case 3:
				sim2Signal.Glyph(L"\uEC39");
				break;
			case 4:
				sim2Signal.Glyph(L"\uEC3A");
				break;
			case 5:
				sim2Signal.Glyph(L"\uEC3B");
				break;
			case 0:
			default:
				sim2Signal.Glyph(L"\u000D");
				break;
			}
		}));
}

void StatusBarWindow::UpdateSim1(Windows::ApplicationModel::Calls::PhoneLine phone_line)
{
	xamlDispatcher.RunAsync(winrt::Windows::UI::Core::CoreDispatcherPriority::Normal, Windows::UI::Core::DispatchedHandler([=]()
		{
			switch (phone_line.NetworkState())
			{
			case Windows::ApplicationModel::Calls::PhoneNetworkState::Unknown:
				//Maybe is waiting for a PIN
				switch (phone_line.CellularDetails().SimState())
				{
				case Windows::ApplicationModel::Calls::PhoneSimState::Unknown: //????
				case Windows::ApplicationModel::Calls::PhoneSimState::PinNotRequired:
				case Windows::ApplicationModel::Calls::PhoneSimState::PinUnlocked:
					//Maybe no signal?
					sim1SignalStatus.Visibility(Visibility::Visible);
					sim1Back.Visibility(Visibility::Visible);
					sim1Signal.Visibility(Visibility::Collapsed);
					sim1Glyph.Visibility(Visibility::Collapsed);

					sim1SignalStatus.Glyph(L"\uE871");

					roamingGrid.Visibility(Visibility::Collapsed);
					break;

				case Windows::ApplicationModel::Calls::PhoneSimState::PinLocked:
				case Windows::ApplicationModel::Calls::PhoneSimState::PukLocked:
					sim1SignalStatus.Visibility(Visibility::Collapsed);
					sim1Back.Visibility(Visibility::Collapsed);
					sim1Signal.Visibility(Visibility::Collapsed);
					sim1Glyph.Visibility(Visibility::Visible);

					sim1Glyph.Glyph(L"\uE875");

					roamingGrid.Visibility(Visibility::Collapsed);
					break;

				case Windows::ApplicationModel::Calls::PhoneSimState::NotInserted:
				case Windows::ApplicationModel::Calls::PhoneSimState::Disabled:
					sim1SignalStatus.Visibility(Visibility::Collapsed);
					sim1Back.Visibility(Visibility::Collapsed);
					sim1Signal.Visibility(Visibility::Collapsed);
					sim1Glyph.Visibility(Visibility::Visible);

					sim1Glyph.Glyph(L"\uE876");

					roamingGrid.Visibility(Visibility::Collapsed);
					break;

				case Windows::ApplicationModel::Calls::PhoneSimState::Invalid:
					sim1SignalStatus.Visibility(Visibility::Collapsed);
					sim1Back.Visibility(Visibility::Collapsed);
					sim1Signal.Visibility(Visibility::Collapsed);
					sim1Glyph.Visibility(Visibility::Visible);

					sim1Glyph.Glyph(L"\uF5AB");

					roamingGrid.Visibility(Visibility::Collapsed);
					break;
				default:
					break;
				}
				break;
			case Windows::ApplicationModel::Calls::PhoneNetworkState::Searching:
			case Windows::ApplicationModel::Calls::PhoneNetworkState::NoSignal:
				sim1SignalStatus.Visibility(Visibility::Visible);
				sim1Back.Visibility(Visibility::Visible);
				sim1Signal.Visibility(Visibility::Collapsed);
				sim1Glyph.Visibility(Visibility::Collapsed);

				sim1SignalStatus.Glyph(L"\uE871");

				roamingGrid.Visibility(Visibility::Collapsed);
				break;
			case Windows::ApplicationModel::Calls::PhoneNetworkState::Deregistered:
			case Windows::ApplicationModel::Calls::PhoneNetworkState::Denied:
				sim1SignalStatus.Visibility(Visibility::Visible);
				sim1Back.Visibility(Visibility::Visible);
				sim1Signal.Visibility(Visibility::Collapsed);
				sim1Glyph.Visibility(Visibility::Collapsed);

				sim1SignalStatus.Glyph(L"\uED2E");

				roamingGrid.Visibility(Visibility::Collapsed);
				break;

			case Windows::ApplicationModel::Calls::PhoneNetworkState::Home:
				sim1SignalStatus.Visibility(Visibility::Collapsed);
				sim1Back.Visibility(Visibility::Visible);
				sim1Signal.Visibility(Visibility::Visible);
				sim1Glyph.Visibility(Visibility::Collapsed);

				roamingGrid.Visibility(Visibility::Collapsed);
				break;

			case Windows::ApplicationModel::Calls::PhoneNetworkState::RoamingInternational:
			case Windows::ApplicationModel::Calls::PhoneNetworkState::RoamingDomestic:
				sim1SignalStatus.Visibility(Visibility::Collapsed);
				sim1Back.Visibility(Visibility::Visible);
				sim1Signal.Visibility(Visibility::Visible);
				sim1Glyph.Visibility(Visibility::Collapsed);

				roamingGrid.Visibility(Visibility::Visible);
				break;
			default:
				break;
			}
		}));
}

void StatusBarWindow::UpdateSim2(Windows::ApplicationModel::Calls::PhoneLine phone_line)
{
	xamlDispatcher.RunAsync(winrt::Windows::UI::Core::CoreDispatcherPriority::Normal, Windows::UI::Core::DispatchedHandler([=]()
		{
			switch (phone_line.NetworkState())
			{
			case Windows::ApplicationModel::Calls::PhoneNetworkState::Unknown:
				//Maybe is waiting for a PIN
				switch (phone_line.CellularDetails().SimState())
				{
				case Windows::ApplicationModel::Calls::PhoneSimState::Unknown: //????
				case Windows::ApplicationModel::Calls::PhoneSimState::PinNotRequired:
				case Windows::ApplicationModel::Calls::PhoneSimState::PinUnlocked:
					//Maybe no signal?
					sim2SignalStatus.Visibility(Visibility::Visible);
					sim2Back.Visibility(Visibility::Visible);
					sim2Signal.Visibility(Visibility::Collapsed);
					sim2Glyph.Visibility(Visibility::Collapsed);

					sim2SignalStatus.Glyph(L"\uE871");

					roamingGrid.Visibility(Visibility::Collapsed);
					break;

				case Windows::ApplicationModel::Calls::PhoneSimState::PinLocked:
				case Windows::ApplicationModel::Calls::PhoneSimState::PukLocked:
					sim2SignalStatus.Visibility(Visibility::Collapsed);
					sim2Back.Visibility(Visibility::Collapsed);
					sim2Signal.Visibility(Visibility::Collapsed);
					sim2Glyph.Visibility(Visibility::Visible);

					sim2Glyph.Glyph(L"\uE875");

					roamingGrid.Visibility(Visibility::Collapsed);
					break;

				case Windows::ApplicationModel::Calls::PhoneSimState::NotInserted:
				case Windows::ApplicationModel::Calls::PhoneSimState::Disabled:
					sim2SignalStatus.Visibility(Visibility::Collapsed);
					sim2Back.Visibility(Visibility::Collapsed);
					sim2Signal.Visibility(Visibility::Collapsed);
					sim2Glyph.Visibility(Visibility::Visible);

					sim2Glyph.Glyph(L"\uE876");

					roamingGrid.Visibility(Visibility::Collapsed);
					break;

				case Windows::ApplicationModel::Calls::PhoneSimState::Invalid:
					sim2SignalStatus.Visibility(Visibility::Collapsed);
					sim2Back.Visibility(Visibility::Collapsed);
					sim2Signal.Visibility(Visibility::Collapsed);
					sim2Glyph.Visibility(Visibility::Visible);

					sim2Glyph.Glyph(L"\uF5AB");

					roamingGrid.Visibility(Visibility::Collapsed);
					break;
				default:
					break;
				}
				break;
			case Windows::ApplicationModel::Calls::PhoneNetworkState::Searching:
			case Windows::ApplicationModel::Calls::PhoneNetworkState::NoSignal:
				sim2SignalStatus.Visibility(Visibility::Visible);
				sim2Back.Visibility(Visibility::Visible);
				sim2Signal.Visibility(Visibility::Collapsed);
				sim2Glyph.Visibility(Visibility::Collapsed);

				sim2SignalStatus.Glyph(L"\uE871");

				roamingGrid.Visibility(Visibility::Collapsed);
				break;
			case Windows::ApplicationModel::Calls::PhoneNetworkState::Deregistered:
			case Windows::ApplicationModel::Calls::PhoneNetworkState::Denied:
				sim2SignalStatus.Visibility(Visibility::Visible);
				sim2Back.Visibility(Visibility::Visible);
				sim2Signal.Visibility(Visibility::Collapsed);
				sim2Glyph.Visibility(Visibility::Collapsed);

				sim2SignalStatus.Glyph(L"\uED2E");

				roamingGrid.Visibility(Visibility::Collapsed);
				break;

			case Windows::ApplicationModel::Calls::PhoneNetworkState::Home:
				sim2SignalStatus.Visibility(Visibility::Collapsed);
				sim2Back.Visibility(Visibility::Visible);
				sim2Signal.Visibility(Visibility::Visible);
				sim2Glyph.Visibility(Visibility::Collapsed);

				roamingGrid.Visibility(Visibility::Collapsed);
				break;

			case Windows::ApplicationModel::Calls::PhoneNetworkState::RoamingInternational:
			case Windows::ApplicationModel::Calls::PhoneNetworkState::RoamingDomestic:
				sim2SignalStatus.Visibility(Visibility::Collapsed);
				sim2Back.Visibility(Visibility::Visible);
				sim2Signal.Visibility(Visibility::Visible);
				sim2Glyph.Visibility(Visibility::Collapsed);

				roamingGrid.Visibility(Visibility::Visible);
				break;
			default:
				break;
			}
		}));
}

void StatusBarWindow::SetupAppBar()
{
	const auto orientation = Utils::GetCurrentOrientation();

	if (orientation == DMDO_DEFAULT || orientation == DMDO_180)
	{
		if (appbarMessageId != -1u)
			Utils::ABSetPos(hwndParent, width * effectiveDpi, height, effectiveDpi, ABE_TOP);
		else
			appbarMessageId = Utils::RegisterAppBar(hwndParent, width * effectiveDpi, height, effectiveDpi, ABE_TOP);
	}
	else if (orientation == DMDO_90)
	{
		if (appbarMessageId != -1u)
			Utils::ABSetPos(hwndParent, width, height, effectiveDpi, ABE_RIGHT);
		else
			appbarMessageId = Utils::RegisterAppBar(hwndParent, width, height, effectiveDpi, ABE_RIGHT);
	}
	else if (orientation == DMDO_270)
	{
		if (appbarMessageId != -1u)
			Utils::ABSetPos(hwndParent, width, height, effectiveDpi, ABE_LEFT);
		else
			appbarMessageId = Utils::RegisterAppBar(hwndParent, width, height, effectiveDpi, ABE_LEFT);
	}

	HandleRotation(base, child);
}