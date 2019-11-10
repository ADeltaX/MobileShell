#include "pch.h"
#include "Wnf.h"
#include "Utils.h"
#include "BaseWindow.h"
#include "NavBarWindow.h"
#include "StatusBarWindow.h"
#include "winrt/Windows.Networking.Connectivity.h"
#include "winrt/Windows.System.Power.h"
#include "winrt/Windows.Devices.Radios.h"
#include "winrt/Windows.Devices.Bluetooth.h"
#include "winrt/Windows.Devices.Enumeration.h"
#include "winrt/Windows.Devices.WiFi.h"
#include "winrt/Windows.ApplicationModel.Calls.h"
#include <Windows.h>
#include <iostream>
#include <thread>
#include "NotificationWindow.h"
#include "dxgi.h"
#include "dxgi1_6.h"

#pragma comment(lib, "dxgi.lib")

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml::Hosting;
using namespace Windows::System::Power;
using namespace Windows::Media::Playback;
using namespace Windows::Devices::Radios;
using namespace Windows::Networking::Connectivity;
using namespace Windows::ApplicationModel::Calls;

NavBarWindow* wind;
StatusBarWindow* statusBarW;
std::vector<Radio> radios;
std::vector<PhoneLine> phoneLines;

std::vector<PhoneLine> GetPhoneLines();
void HandlePhoneLines(std::vector<PhoneLine> phone_lines);
void NetworkStatusChange();
void GetWifiSignal();
void SetupPowerManager();
void SetupRadios();
void SetRadioStatus(Radio radio);
void MoveNotificationsOnTop();
void OnTabletModeChanged(bool istabletmode);
void GlobalSetupAppBar();
void ForceFullCoverStartScreen();


// .imrsiv also REQUIRES IMAGE_DLLCHARACTERISTICS_FORCE_INTEGRITY, MEANING IT SHOULD BE SIGNED (also, Windows certificate) --> no window(s) on other ZBIDs
// Or we can cheat by launching a new (suspended) RuntimeBroker process and then inject a dll

int CALLBACK WinMain(_In_ HINSTANCE hInstance, _In_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	//Useful to change on-the-fly dpi
	//SystemParametersInfo(SPI_SETLOGICALDPIOVERRIDE, 0, LPVOID(nullptr), 1);

	const auto m_singleInstanceMutex = CreateMutex(NULL, TRUE, L"MobileShellPlus");

	if (m_singleInstanceMutex == NULL || GetLastError() == ERROR_ALREADY_EXISTS) {
		MessageBox(NULL, L"MobileShell is already open, switch to tablet mode to see it!", L"uWu", 0);
		return 0;
	}

	DXGIDeclareAdapterRemovalSupport();
	auto res = SetProcessShutdownParameters(1, SHUTDOWN_NORETRY);

	init_apartment();

	auto windows_xaml_manager = WindowsXamlManager::InitializeForCurrentThread();

	wind = new NavBarWindow(hInstance);
	statusBarW = new StatusBarWindow(hInstance);

	statusBarW->PreBuild();
	wind->PreBuild();

	if (Wnf::IsTabletMode())
		OnTabletModeChanged(true);

	Wnf::SubscribeWnf(WNF_TMCN_ISTABLETMODE, WnfCallback, NULL);
	Wnf::SubscribeWnf(WNF_SHEL_NOTIFICATIONS, WnfCallback, NULL);
	Wnf::SubscribeWnf(WNF_SHEL_QUIETHOURS_ACTIVE_PROFILE_CHANGED, WnfCallback, NULL);
	Wnf::SubscribeWnf(WNF_CELL_SIGNAL_STRENGTH_BARS_CAN0, WnfCallback, NULL);
	Wnf::SubscribeWnf(WNF_CELL_SIGNAL_STRENGTH_BARS_CAN1, WnfCallback, NULL);

	phoneLines = GetPhoneLines();
	HandlePhoneLines(phoneLines);
	NetworkStatusChange();
	SetupPowerManager();
	SetupRadios();
	wind->SetupHaptics();
	
	std::thread TA(MoveNotificationsOnTop);
	std::thread TWifi(GetWifiSignal);
	std::thread TCapra(GlobalSetupAppBar);

	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return int(msg.wParam);
}

void GlobalSetupAppBar()
{
	while (true)
	{
		if (Wnf::IsTabletMode())
		{
			OnTabletModeChanged(true);
			//ForceFullCoverStartScreen();
		}

		Sleep(5000);
	}
}

void ForceFullCoverStartScreen()
{
	RECT rect = { 0 };
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);

	const auto hwnd = FindWindowEx(nullptr, nullptr, L"Windows.UI.Core.CoreWindow", L"Start");
	if (hwnd)
	{
		SetWindowPos(hwnd, nullptr, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_NOSENDCHANGING);
	}
}

void HandlePhoneLines(std::vector<PhoneLine> phone_lines)
{
	if (phone_lines.size() > 1)
		statusBarW->SetDualSim();

	for (const auto& phone_line : phone_lines)
	{
		if (phone_line.CellularDetails().SimSlotIndex() == 0)
			statusBarW->UpdateSim1(phone_line);
		else if (phone_line.CellularDetails().SimSlotIndex() == 1)
			statusBarW->UpdateSim2(phone_line);
		
		/*int index = static_cast<int>(phone_line.NetworkState());
		MessageBoxA(NULL, std::to_string(index).c_str(), "OWO", 0);*/
		
		phone_line.LineChanged([](PhoneLine const& o, IInspectable const&)
			{
				if (o.CellularDetails().SimSlotIndex() == 0)
					statusBarW->UpdateSim1(o);
				else if (o.CellularDetails().SimSlotIndex() == 1)
					statusBarW->UpdateSim2(o);
			});
	}
}

void SetupPowerManager()
{
	statusBarW->UpdateBatteryPercentage();

	PowerManager::BatteryStatusChanged([](IInspectable const&, IInspectable const&)
		{
			statusBarW->UpdateBatteryPercentage();
		});

	PowerManager::EnergySaverStatusChanged([](IInspectable const&, IInspectable const&)
		{
			statusBarW->UpdateBatteryPercentage();
		});

	PowerManager::RemainingChargePercentChanged([](IInspectable const&, IInspectable const&)
		{
			statusBarW->UpdateBatteryPercentage();
		});
}

void SetupRadios()
{
	auto wew = Windows::Devices::Radios::Radio::GetRadiosAsync().get();

	const auto radio_state_changed = [](Radio const& radio, IInspectable const& args)
	{	
		SetRadioStatus(radio);
	};

	for (const auto& item : wew)
	{
		item.StateChanged(radio_state_changed);
		radios.push_back(item);
		SetRadioStatus(item);
	}

	/*const hstring filter = Windows::Devices::Bluetooth::BluetoothDevice::GetDeviceSelector();
	auto test = Windows::Devices::Enumeration::DeviceInformation::FindAllAsync(filter).get();*/
}

void SetRadioStatus(Radio radio)
{
	if (radio.Kind() == RadioKind::MobileBroadband)
	{
		//99%, not exactly sure --> Airplane mode
		if (radio.State() != RadioState::On)
		{
			statusBarW->SetAirplaneMode(true);
		}
		else
		{
			statusBarW->SetAirplaneMode(false);
		}
	}
	else if (radio.Kind() == RadioKind::Bluetooth)
	{
		if (radio.State() != RadioState::On)
		{
			statusBarW->SetBluetooth(false);
		}
		else
		{
			statusBarW->SetBluetooth(true);
		}
	}
	else if (radio.Kind() == RadioKind::WiFi)
	{
		if (radio.State() != RadioState::On)
		{
			statusBarW->SetWiFi(false);
		}
		else
		{
			statusBarW->SetWiFi(true);
		}
	}
}

void MoveNotificationsOnTop()
{
	while (true)
	{
		const auto hwnd = FindWindowEx(nullptr, nullptr, L"Windows.UI.Core.CoreWindow", L"New Notification");
		if (hwnd)
		{
			double mon_dpi;
			int mon_height;
			int mon_width;
			Utils::GetMonitorSizeAndDpiFromWindow(hwnd, &mon_dpi, &mon_height, &mon_width);

			RECT rect = {};
			GetWindowRect(hwnd, &rect);

			const int notification_width = rect.right - rect.left;

			SetWindowPos(hwnd, nullptr, ((double(mon_width) - notification_width) / 2.0), -70 * mon_dpi + 24.0 * mon_dpi, 0, 0, SWP_NOSIZE);
		}

		Sleep(10);
	}
}

std::vector<PhoneLine> GetPhoneLines()
{
	PhoneCallStore store = PhoneCallManager::RequestStoreAsync().get();
	auto watcher = store.RequestLineWatcher();
	std::vector<PhoneLine> phone_lines;
	bool wait = true;

	watcher.LineAdded([&](IInspectable const&, PhoneLineWatcherEventArgs const& args)
	{
		const auto line = PhoneLine::FromIdAsync(args.LineId()).get();
		phone_lines.push_back(line);
	});

	watcher.EnumerationCompleted([&](IInspectable const&, IInspectable const&)
		{
			wait = false;
		});

	watcher.Stopped([&](IInspectable const&, IInspectable const&)
		{
			wait = false;
		});
	
	watcher.Start();

	//Surely not the best way.
	while (wait)
	{
		Sleep(1);
	}

	watcher.Stop();

	std::vector<PhoneLine> returned_lines;
	
	for (auto phone_line : phone_lines)
		if (phone_line != nullptr && phone_line.Transport() == PhoneLineTransport::Cellular)
			returned_lines.push_back(phone_line);

	return returned_lines;
}

void NetworkStatusChange()
{
	NetworkInformation::NetworkStatusChanged([](IInspectable const& object)
		{
			const auto profile = NetworkInformation::GetInternetConnectionProfile();
			if (profile != nullptr)
			{
				if (profile.IsWlanConnectionProfile())
				{
					//GET WIFI STRENGTH - from 0 to 5
					auto strength = profile.GetSignalBars().Value();
					statusBarW->SetWiFiSignal(strength);
				}
				else
				{
					//Probably not connected, set wifi strength to 0
					statusBarW->SetWiFiSignal(0);
				}

				if (profile.IsWwanConnectionProfile())
				{
					//Things becomes more interesting
				}
			}
			else
			{
				//No connectivity. Reset wifi signal.
				statusBarW->SetWiFiSignal(0);
			}
		});
}

void GetWifiSignal()
{
	while (true)
	{
		//I'm sure there is a better way.
		const auto profile = NetworkInformation::GetInternetConnectionProfile();

		if (profile != nullptr)
		{
			if (profile.IsWlanConnectionProfile())
			{
				//GET WIFI STRENGTH - from 0 to 5
				auto strength = profile.GetSignalBars().Value();
				statusBarW->SetWiFiSignal(strength);
			}
			else
			{
				//Probably not connected, set wifi strength to 0
				statusBarW->SetWiFiSignal(0);
			}
		}
		else
		{
			//No connectivity. Reset wifi signal.
			statusBarW->SetWiFiSignal(0);
		}
		

		Sleep(2000);
	}
}

void OnTabletModeChanged(bool istabletmode)
{
	if (istabletmode)
	{
		//Utils::RemoveWinGestures();
		//Utils::SetWinTaskbarIcons(WinTaskbarIconSize::TSB_SMALL);
		Utils::SetWinTaskbarState(WinTaskbarState::AutoHide);
		Utils::SetWinTaskbarVisible(false);

		statusBarW->SetupAppBar(true);
		wind->SetupAppBar(true);

		statusBarW->Show();
		wind->Show();
	}
	else
	{
		//Utils::SetWinTaskbarIcons(WinTaskbarIconSize::TSB_NORMAL);
		Utils::SetWinTaskbarState(WinTaskbarState::OnTop);
		Utils::SetWinTaskbarVisible(true);

		statusBarW->SetupAppBar(false);
		wind->SetupAppBar(false);

		statusBarW->Hide();
		wind->Hide();
	}	
}

NTSTATUS NTAPI WnfCallback(const ULONG64 state_name, void* p2, void* p3, void* callbackContext, void* buffer, ULONG bufferSize)
{
	const auto p = static_cast<byte*>(buffer);
	std::vector<unsigned char> wnf_state_buffer(bufferSize);
	memcpy(wnf_state_buffer.data(), p, bufferSize);

	if (state_name == WNF_TMCN_ISTABLETMODE)
	{
		const auto is_tablet_mode = Wnf::ToBool(wnf_state_buffer.data());
		OnTabletModeChanged(is_tablet_mode);
	}
	else if (state_name == WNF_SHEL_NOTIFICATIONS)
	{
		const auto num_notif = Wnf::ToInt32(wnf_state_buffer.data());
		statusBarW->SetNotifications(num_notif);
	}
	else if (state_name == WNF_SHEL_QUIETHOURS_ACTIVE_PROFILE_CHANGED)
	{
		const auto profile_state = *static_cast<DWORD*>(buffer);

		FocusAssistStatus fas = FocusAssistStatus::Off;

		if (profile_state == 0x0)
			fas = FocusAssistStatus::Off;
		else if (profile_state == 0x1)
			fas = FocusAssistStatus::PriorityOnly;
		else if (profile_state == 0x2)
			fas = FocusAssistStatus::AlarmsOnly;

		statusBarW->SetFocusAssist(fas);
	}
	else if (state_name == WNF_LFS_STATE)
	{
		const auto is_location_in_use = Wnf::ToBool(wnf_state_buffer.data());
		statusBarW->SetLocation(is_location_in_use);
	}
	else if (state_name == WNF_CNET_WIFI_ACTIVITY)
	{
		const auto wifi_activity = Wnf::ToBool(wnf_state_buffer.data());
		MessageBox(nullptr, L"UPDATE", L"UWU", 0);
		//statusBarW->SetVPN(wifi_activity);
	}
	else if (state_name == WNF_VPN_CLIENT_CONNECTIVITY_STATUS)
	{
		//TODO: it's more than bool. It's a structure. Needs RE
		/*const auto is_vpn_connection_established = Wnf::ToBool(wnf_state_buffer.data());
		statusBarW->SetVPN(is_vpn_connection_established);*/
	}
	else if (state_name == WNF_CELL_SIGNAL_STRENGTH_BARS_CAN0)
	{
		const auto signal_strength = *static_cast<DWORD*>(buffer);
		statusBarW->SetSim1Signal(signal_strength);
	}
	else if (state_name == WNF_CELL_SIGNAL_STRENGTH_BARS_CAN1)
	{
		const auto signal_strength = *static_cast<DWORD*>(buffer);
		statusBarW->SetSim2Signal(signal_strength);
	}
	else if (state_name == WNF_WIFI_CONNECTION_SCORE)
	{
		//lol, useless.
	}
	else if (state_name == WNF_SHEL_TOAST_PUBLISHED)
	{
		//Move notification window on top (experimental) | untested outside EN_* Windows locale
		
		//not working. lol.
	}

	return 0;
}
