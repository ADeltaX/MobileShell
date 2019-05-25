using MobileShell.Classes;
using MobileShell.Controls;
using MobileShell.Enums;
using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Windows;
using System.Windows.Forms;
using System.Windows.Interop;
using System.Windows.Media;
using System.Windows.Threading;
using Windows.ApplicationModel.Calls;
using Windows.Networking.Connectivity;
using Windows.Networking.NetworkOperators;
using Windows.System.Power;
using static MobileShell.Classes.NativeMethods;

namespace MobileShell
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class StatusBarWindow : Window
    {
        int appbarMessageId = -1;
        const string BATTERY_NOT_PRESENT_GLYPH = "\uEC02";

        //remember this
        //MobileBroadbandTransmissionStateChangedEventArgs.IsTransmitting == USEFUL.

        public StatusBarWindow()
        {
            ShowActivated = false;
            ShowInTaskbar = false;
            Loaded += Window_Loaded;

            InitializeComponent();

            Width = SystemParameters.PrimaryScreenWidth;
            Top = 0;
            Left = 0;
        }

        private SignalStrength? IntToSignalStrength(int signalStrength)
        {
            if (signalStrength == 0)
                return SignalStrength.Strength_0;
            else if (signalStrength == 1)
                return SignalStrength.Strength_1;
            else if (signalStrength == 2)
                return SignalStrength.Strength_2;
            else if (signalStrength == 3)
                return SignalStrength.Strength_3;
            else if (signalStrength == 4)
                return SignalStrength.Strength_4;
            else if (signalStrength == 5)
                return SignalStrength.Strength_5;
            else
                return null;
        }

        public void AddPhoneLine(PhoneLine phoneLine, bool isDualSim)
        {
            Dispatcher.BeginInvoke(DispatcherPriority.Render, new Action(() =>
            {
                SignalStrength? snStng = null;

                if (phoneLine.CellularDetails.SimSlotIndex == 0)
                    snStng = IntToSignalStrength(WnfQueries.QueryCan0CellularSignalStrength());
                else if (phoneLine.CellularDetails.SimSlotIndex == 1)
                    snStng = IntToSignalStrength(WnfQueries.QueryCan1CellularSignalStrength());

                CellularIcon cellularIcon = new CellularIcon
                {
                    PhoneSimState = phoneLine.CellularDetails.SimState,
                    IsDualSim = isDualSim,
                    DualSimStatusNumber = phoneLine.CellularDetails.SimSlotIndex,
                    PhoneGuid = phoneLine.Id,
                    PhoneNetworkState = phoneLine.NetworkState,
                    SignalStrength = snStng.HasValue ? snStng.Value : SignalStrength.Strength_0
                };

                phoneLinesStackPanel.Children.Add(cellularIcon);
            }));
        }

        public void UpdateSignalStrength(int signalStrength, int simSlotIndex)
        {
            Dispatcher.BeginInvoke(DispatcherPriority.Render, new Action(() =>
            {
                CellularIcon cellIcon = null;

                foreach (var item in phoneLinesStackPanel.Children)
                {
                    if (((CellularIcon)item).DualSimStatusNumber == simSlotIndex)
                    {
                        cellIcon = (CellularIcon)item;
                        break;
                    }
                }

                if (cellIcon != null)
                {
                    SignalStrength sgStrn = IntToSignalStrength(signalStrength).Value;
                    cellIcon.SignalStrength = sgStrn;
                }
            }));

        }

        public void SetVisibility(Visibility vis) => Dispatcher.BeginInvoke(DispatcherPriority.Send, new Action(() => Visibility = vis));

        public void SetupAppBar()
        {
            Dispatcher.BeginInvoke(DispatcherPriority.Send, new Action(() => 
            {
                WindowInteropHelper wndHelper = new WindowInteropHelper(this);

                int exStyle = (int)GetWindowLong(wndHelper.Handle, (int)GetWindowLongFields.GWL_EXSTYLE);

                exStyle |= (int)ExtendedWindowStyles.WS_EX_TOOLWINDOW;
                SetWindowLong(wndHelper.Handle, (int)GetWindowLongFields.GWL_EXSTYLE, (IntPtr)exStyle);

                appbarMessageId = AppBar.RegisterBar(this, Screen.PrimaryScreen, Width * App.DPI, Height * App.DPI, ABEdge.ABE_TOP); //Height
            }));
        }

        public void UpdateAppBar()
        {
            Dispatcher.BeginInvoke(DispatcherPriority.Send, new Action(() =>
                AppBar.ABSetPos(this, Screen.PrimaryScreen, Width * App.DPI, Height * App.DPI, ABEdge.ABE_TOP)));
        }

        public void UnSetupAppBar()
        {
            Dispatcher.BeginInvoke(DispatcherPriority.Send, new Action(() =>
            {
                APPBARDATA abd = new APPBARDATA();
                abd.cbSize = Marshal.SizeOf(typeof(APPBARDATA));
                IntPtr handle = new WindowInteropHelper(this).Handle;
                abd.hWnd = handle;
                SHAppBarMessage((int)ABMsg.ABM_REMOVE, ref abd);
            }));
        }

        //TODO
        private void Turn()
        {
            appbarMessageId = AppBar.RegisterBar(this, Screen.PrimaryScreen, 48 * App.DPI, Screen.PrimaryScreen.Bounds.Height / App.DPI, ABEdge.ABE_LEFT);

            gridStatusBar.RenderTransformOrigin = new Point(0.5, 0.5);
            gridStatusBar.LayoutTransform = new RotateTransform(-90);

            clockTextBox.RenderTransformOrigin = new Point(0.5, 0.5);
            clockTextBox.LayoutTransform = new RotateTransform(90);

            batteryGlyph.RenderTransformOrigin = new Point(0.5, 0.5);
            batteryGlyph.LayoutTransform = new RotateTransform(90);

            batteryPercentage.RenderTransformOrigin = new Point(0.5, 0.5);
            batteryPercentage.LayoutTransform = new RotateTransform(90);

            focusGlyph.RenderTransformOrigin = new Point(0.5, 0.5);
            focusGlyph.LayoutTransform = new RotateTransform(90);

            locationGlyph.RenderTransformOrigin = new Point(0.5, 0.5);
            locationGlyph.LayoutTransform = new RotateTransform(90);

            notificationGlyph.RenderTransformOrigin = new Point(0.5, 0.5);
            notificationGlyph.LayoutTransform = new RotateTransform(90);

            ethernetGlyph.RenderTransformOrigin = new Point(0.5, 0.5);
            ethernetGlyph.LayoutTransform = new RotateTransform(90);

            wifiComposedGlyphsGrid.RenderTransformOrigin = new Point(0.5, 0.5);
            wifiComposedGlyphsGrid.LayoutTransform = new RotateTransform(90);

            roamingGlyph.RenderTransformOrigin = new Point(0.5, 0.5);
            roamingGlyph.LayoutTransform = new RotateTransform(90);

            //mobileConnectionComposedGlyphsGrid.RenderTransformOrigin = new Point(0.5, 0.5);
            //mobileConnectionComposedGlyphsGrid.LayoutTransform = new RotateTransform(90);

            //mobileConnectionTypeText.RenderTransformOrigin = new Point(0.5, 0.5);
            //mobileConnectionTypeText.LayoutTransform = new RotateTransform(90);
        }

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            //new AcrylicBlur(this).EnableBlur();

            //Update the battery percentage.
            UpdateBatteryIconAndPercentage();

            UpdateNetworkState();

            //CLOCK
            DispatcherTimer dispClock = new DispatcherTimer(TimeSpan.FromMilliseconds(1000), DispatcherPriority.Render, delegate
            {
                clockTextBox.Text = DateTime.Now.ToString("HH:mm"); //+- 1s
            }, Dispatcher);

            //TODO: better way
            //Specific update for SIM/MobileBroadband because it's too expensive
            //DispatcherTimer dispMobile = new DispatcherTimer(TimeSpan.FromMilliseconds(2500), DispatcherPriority.Background, delegate
            //{
            //    if (hasModem = HasModem())
            //    {
            //        mobileConnectionStackPanel.Visibility = Visibility.Visible;
            //        if (hasSIM = HasSIM())
            //        {
            //            noSimGlyph.Visibility = Visibility.Collapsed;
            //            mobileConnectionSignalBase.Visibility = Visibility.Visible;

            //            int strength = WnfQueries.QueryCan0CellularSignalStrength();
            //            mobileConnectionSignal.Visibility = Visibility.Visible;
            //            if (strength <= 0)
            //                mobileConnectionSignal.Visibility = Visibility.Collapsed;
            //            else if (strength == 1)
            //                mobileConnectionSignal.UnicodeString = "\uEC37";
            //            else if (strength == 2)
            //                mobileConnectionSignal.UnicodeString = "\uEC38";
            //            else if (strength == 3)
            //                mobileConnectionSignal.UnicodeString = "\uEC39";
            //            else if (strength == 4)
            //                mobileConnectionSignal.UnicodeString = "\uEC3A";
            //            else if (strength == 5)
            //                mobileConnectionSignal.UnicodeString = "\uEC3B";
            //        }
            //        else
            //        {
            //            noSimGlyph.Visibility = Visibility.Visible;
            //            mobileConnectionSignalBase.Visibility = Visibility.Collapsed;
            //            mobileConnectionSignal.Visibility = Visibility.Collapsed;
            //            mobileConnectionTypeText.Visibility = Visibility.Collapsed;
            //        }
            //    }
            //    else
            //    {
            //        mobileConnectionStackPanel.Visibility = Visibility.Collapsed;
            //        mobileConnectionTypeText.Visibility = Visibility.Collapsed;
            //    }

            //}, Dispatcher);
        }

        #region Update Battery Icon w/ percentage
        
        public void UpdateBatteryIconAndPercentage()
        {
            Dispatcher.BeginInvoke(DispatcherPriority.Render, new Action(() => 
            {
                if (PowerManager.BatteryStatus == BatteryStatus.NotPresent)
                {
                    batteryGlyph.UnicodeString = BATTERY_NOT_PRESENT_GLYPH;
                    batteryPercentage.Visibility = Visibility.Collapsed;
                }
                else if (PowerManager.BatteryStatus == BatteryStatus.Charging)
                {
                    batteryPercentage.Visibility = Visibility.Visible;
                    batteryPercentage.Text = PowerManager.RemainingChargePercent + "%";
                    batteryGlyph.UnicodeString = GetBatteryStatusGlyph(PowerManager.RemainingChargePercent, true, false);
                }
                else if (PowerManager.BatteryStatus == BatteryStatus.Discharging)
                {
                    batteryPercentage.Visibility = Visibility.Visible;
                    batteryPercentage.Text = PowerManager.RemainingChargePercent + "%";

                    if (PowerManager.EnergySaverStatus == EnergySaverStatus.On)
                        batteryGlyph.UnicodeString = GetBatteryStatusGlyph(PowerManager.RemainingChargePercent, false, true);
                    else
                        //Discharging - TODO: add the animation again
                        batteryGlyph.UnicodeString = GetBatteryStatusGlyph(PowerManager.RemainingChargePercent, false, false);
                }
                else if (PowerManager.BatteryStatus == BatteryStatus.Idle)
                {
                    //Plugged in but not charging
                    batteryPercentage.Text = PowerManager.RemainingChargePercent + "%";
                    batteryGlyph.UnicodeString = GetBatteryStatusGlyph(PowerManager.RemainingChargePercent, true, false);
                }
            }));
        }

        /// <summary>
        /// Get the unicode string of the glyph given the current battery, if it's charging, energy saver enabled or normal.
        /// </summary>
        /// <param name="currentBattery">Battery percentage from 0 to 100.</param>
        /// <param name="charging">true to return the charging battery glyph.</param>
        /// <param name="energysaver">true to return the energy saving battery glyph. Charging bool value must be false.</param>
        private string GetBatteryStatusGlyph(int currentBattery, bool charging, bool energysaver)
        {
            if (currentBattery <= 5)
                return charging ? "\uEBAB" : (energysaver ? "\uEBB6" : "\uEBA0");
            else if (currentBattery > 5 && currentBattery <= 10)
                return charging ? "\uEBAC" : (energysaver ? "\uEBB7" : "\uEBA1");
            else if (currentBattery > 10 && currentBattery <= 20)
                return charging ? "\uEBAD" : (energysaver ? "\uEBB8" : "\uEBA2");
            else if (currentBattery > 20 && currentBattery <= 30)
                return charging ? "\uEBAE" : (energysaver ? "\uEBB9" : "\uEBA3");
            else if (currentBattery > 30 && currentBattery <= 40)
                return charging ? "\uEBAF" : (energysaver ? "\uEBBA" : "\uEBA4");
            else if (currentBattery > 40 && currentBattery <= 50)
                return charging ? "\uEBB0" : (energysaver ? "\uEBBB" : "\uEBA5");
            else if (currentBattery > 50 && currentBattery <= 60)
                return charging ? "\uEBB1" : (energysaver ? "\uEBBC" : "\uEBA6");
            else if (currentBattery > 60 && currentBattery <= 70)
                return charging ? "\uEBB2" : (energysaver ? "\uEBBD" : "\uEBA7");
            else if (currentBattery > 70 && currentBattery <= 80)
                return charging ? "\uEBB3" : (energysaver ? "\uEBBE" : "\uEBA8");
            else if (currentBattery > 80 && currentBattery < 95)
                return charging ? "\uEBB4" : (energysaver ? "\uEBBF" : "\uEBA9");
            else
                return charging ? "\uEBB5" : (energysaver ? "\uEBC0" : "\uEBAA");
        }

        #endregion

        public void UpdateNetworkState()
        {
            //CONNECTION TYPES - TODO [ROAMING]
            Dispatcher.BeginInvoke(DispatcherPriority.Render, new Action(() =>
            {
                var connectionType = GetCurrentConnection();
                if (connectionType.Key == ConnectionType.Ethernet)
                {
                    wifiGrid.Visibility = Visibility.Collapsed;
                    ethernetGrid.Visibility = Visibility.Visible;
                    //mobileConnectionTypeText.Visibility = Visibility.Collapsed;
                }
                else if (connectionType.Key == ConnectionType.Unknown)
                {
                    wifiGrid.Visibility = Visibility.Collapsed;
                    ethernetGrid.Visibility = Visibility.Collapsed;
                    //mobileConnectionTypeText.Visibility = Visibility.Collapsed;
                }
                else if (connectionType.Key == ConnectionType.Wlan)
                {
                    wifiGrid.Visibility = Visibility.Visible;
                    wifiSignals.Visibility = Visibility.Visible;
                    //mobileConnectionTypeText.Visibility = Visibility.Collapsed;

                    if (connectionType.Value == 255)
                        wifiGrid.Visibility = Visibility.Collapsed;
                    else if (connectionType.Value >= 4)
                        wifiSignals.UnicodeString = "\uEC3F";
                    else if (connectionType.Value == 3)
                        wifiSignals.UnicodeString = "\uEC3E";
                    else if (connectionType.Value == 2)
                        wifiSignals.UnicodeString = "\uEC3D";
                    else if (connectionType.Value <= 1)
                        wifiSignals.UnicodeString = "\uEC3C";

                    ethernetGrid.Visibility = Visibility.Collapsed;
                }
                else if (connectionType.Key == ConnectionType.Wwan)
                {
                    wifiGrid.Visibility = Visibility.Collapsed;

                    var x = GetWwanDataClass();
                    //if (x.HasValue)
                    //{
                    //    mobileConnectionTypeText.Visibility = Visibility.Visible;
                    //    switch (x.Value)
                    //    {
                    //        case WwanDataClass.Gprs:
                    //            mobileConnectionTypeText.Text = "G";
                    //            break;
                    //        case WwanDataClass.Edge:
                    //            mobileConnectionTypeText.Text = "E";
                    //            break;
                    //        case WwanDataClass.Cdma1xRtt:
                    //            mobileConnectionTypeText.Text = "1X";
                    //            break;
                    //        case WwanDataClass.Umts:
                    //        case WwanDataClass.Cdma1xEvdv:
                    //        case WwanDataClass.Cdma3xRtt:
                    //            mobileConnectionTypeText.Text = "3G";
                    //            break;
                    //        case WwanDataClass.Cdma1xEvdo:
                    //        case WwanDataClass.Cdma1xEvdoRevA:
                    //        case WwanDataClass.Cdma1xEvdoRevB:
                    //            mobileConnectionTypeText.Text = "DO";
                    //            break;
                    //        case WwanDataClass.Hsdpa:
                    //        case WwanDataClass.Hsupa:
                    //            mobileConnectionTypeText.Text = "H";
                    //            break;
                    //        case WwanDataClass.CdmaUmb:
                    //        case WwanDataClass.LteAdvanced:
                    //            mobileConnectionTypeText.Text = "L";
                    //            break;
                    //        default:
                    //            mobileConnectionTypeText.Visibility = Visibility.Collapsed;
                    //            break;
                    //    }
                    //}
                    //else
                    //{
                    //    mobileConnectionTypeText.Visibility = Visibility.Collapsed;
                    //}

                    ethernetGrid.Visibility = Visibility.Collapsed;
                }
            }));
        }

        private static WwanDataClass? GetWwanDataClass()
        {
            var temp = NetworkInformation.GetInternetConnectionProfile();
            if (temp == null || temp.WwanConnectionProfileDetails == null)
                return null;

            return temp.WwanConnectionProfileDetails.GetCurrentDataClass();
        }

        private static KeyValuePair<ConnectionType, byte> GetCurrentConnection()
        {
            var temp = NetworkInformation.GetInternetConnectionProfile();
            if (temp == null)
                return new KeyValuePair<ConnectionType, byte>(ConnectionType.Disconnected, new byte[] { 0xFF }[0]);

            ConnectionType type = ConnectionType.Unknown;

            if (temp.IsWlanConnectionProfile)
                type = ConnectionType.Wlan;
            else if (temp.IsWwanConnectionProfile)
                type = ConnectionType.Wwan;
            else if (temp.ProfileName == "Ethernet")
                type = ConnectionType.Ethernet;

            try
            {
                return new KeyValuePair<ConnectionType, byte>(type, temp.GetSignalBars() ?? new byte[] { 0x0 }[0]);
            }
            catch (Exception)
            {
                return new KeyValuePair<ConnectionType, byte>(type, new byte[] { 0x0 }[0]);
            }
        }

        #region Update Status Icons w/ dispatcher

        public void UpdateFocusAssist(FocusAssistStatus focusAssistStatus)
        {
            Dispatcher.BeginInvoke(DispatcherPriority.Render, new Action(() =>
            {
                if (focusAssistStatus == FocusAssistStatus.Off)
                    focusGrid.Visibility = Visibility.Collapsed;
                else if (focusAssistStatus == FocusAssistStatus.PriorityOnly || focusAssistStatus == FocusAssistStatus.AlarmsOnly)
                    focusGrid.Visibility = Visibility.Visible;
            }));
        }

        public void UpdateLocation(bool locationInUse)
        {
            Dispatcher.BeginInvoke(DispatcherPriority.Render, new Action(() =>
            {
                if (locationInUse)
                    locationGrid.Visibility = Visibility.Visible;
                else
                    locationGrid.Visibility = Visibility.Collapsed;
            }));
        }

        public void UpdateNotification(int unreadNotificationCount)
        {
            Dispatcher.BeginInvoke(DispatcherPriority.Render, new Action(() =>
            {
                if (unreadNotificationCount > 0)
                    notificationGrid.Visibility = Visibility.Visible;
                else
                    notificationGrid.Visibility = Visibility.Collapsed;
            }));
        }

        #endregion

        //Since swipe is hard to achieve and almost useless considering we can't move the action center
        private void Button_Click(object sender, RoutedEventArgs e) => ShellKeyCombo(VirtualKeyShort.LWIN, VirtualKeyShort.KEY_A);
    }
}
