using MobileShell.Classes;
using System;
using System.Collections.Generic;
using System.IO;
using System.Runtime.InteropServices;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Forms;
using System.Windows.Media;
using Windows.ApplicationModel.Calls;
using Windows.Data.Xml.Dom;
using Windows.Foundation.Metadata;
using Windows.Networking.Connectivity;
using Windows.System.Power;
using Windows.UI.Notifications;
using static MobileShell.Classes.NativeMethods;
using MessageBox = System.Windows.MessageBox;

namespace MobileShell
{
    /// <summary>
    /// Interaction logic for App.xaml
    /// </summary>
    public partial class App : System.Windows.Application
    {
        public static float DPI { get; set; } = 1F;
        public static bool IsTabletMode { get; set; }
        public static HookEngine Kbh { get; set; }

        //We don't wanna GC to collect this, right? *yells*
        private WnfQueries.WnfUserCallback wnfCallback;

        //We DON'T want to have more than one instances opened
        static Mutex mutex = new Mutex(true, "f93fa850-2d5e-4316-bc9e-MOBILESHELL");

        
        private void SendToast(string msg)
        {
            XmlDocument toastXml = ToastNotificationManager.GetTemplateContent(ToastTemplateType.ToastText01);

            XmlNodeList stringElements = toastXml.GetElementsByTagName("text");
            for (int i = 0; i < stringElements.Length; i++)
                stringElements[i].AppendChild(toastXml.CreateTextNode(msg));

            ToastNotification toast = new ToastNotification(toastXml) {  Priority = ToastNotificationPriority.High };

            ToastNotificationManager.CreateToastNotifier("MobileShell").Show(toast);
        }

        private static ThemeEngine themeEngine;

        #region Main instances of Window(s)

        private static StatusBarWindow stBar;
        private static NavbarWindow tkBar;
        private static VolumeAudioFlyout vlFly;

        #endregion

        private void Application_Startup(object sender, StartupEventArgs e)
        {
            if (!mutex.WaitOne(TimeSpan.Zero, true))
            {
                SendToast("MobileShell is already running!");
                Environment.Exit(0);
            }
            else
            {
                if (WnfQueries.QueryIsTabletMode())
                    SendToast("MobileShell started!");
                else
                    SendToast("MobileShell started! Switch to tablet mode to use it!");
            }

            System.Windows.Forms.Application.EnableVisualStyles();
            AppDomain.CurrentDomain.UnhandledException += CurrentDomain_UnhandledException;
            ShutdownMode = ShutdownMode.OnExplicitShutdown;

            themeEngine = new ThemeEngine();

            stBar = new StatusBarWindow();
            tkBar = new NavbarWindow();

            stBar.Show();
            tkBar.Show();

            //TODO: Finish.
            //vlFly = new VolumeAudioFlyout();
            //Kbh = new HookEngine();
            //Kbh.OnKeyPressed += kbh_OnKeyPressed;
            //Kbh.OnKeyUnpressed += kbh_OnKeyUnpressed;
            //Kbh.HookKeyboard();


            ////TODO: REMOVE
            //stBar.SetVisibility(Visibility.Visible);
            //tkBar.SetVisibility(Visibility.Visible);
            ////stBar.UpdateAppBar(); //STACK OVERFLOW EXCEPTION !!!!111!!!11!
            //stBar.SetupAppBar();
            ////tkBar.UpdateAppBar();
            //tkBar.SetupAppBar();
            //HideExplorerTaskbar();
            //////END TODO.


            DPI = (float)VisualTreeHelper.GetDpi(stBar).DpiScaleX;

            PowerManager.BatteryStatusChanged += (_, __) => stBar?.UpdateBatteryIconAndPercentage();
            PowerManager.EnergySaverStatusChanged += (_, __) => stBar?.UpdateBatteryIconAndPercentage();
            PowerManager.RemainingChargePercentChanged += (_, __) => stBar?.UpdateBatteryIconAndPercentage();
            NetworkInformation.NetworkStatusChanged += (_) => stBar?.UpdateNetworkState();

            if (ApiInformation.IsTypePresent("Windows.ApplicationModel.Calls.PhoneLine"))
                _ = MonitorPhoneAsync();
            
            wnfCallback = WnfStateUpdates;

            WnfQueries.SubscribeWnf(WnfQueries.WNF_SHEL_QUIETHOURS_ACTIVE_PROFILE_CHANGED, wnfCallback);
            WnfQueries.SubscribeWnf(WnfQueries.WNF_SHEL_NOTIFICATIONS, wnfCallback);
            WnfQueries.SubscribeWnf(WnfQueries.WNF_LFS_STATE, wnfCallback);
            WnfQueries.SubscribeWnf(WnfQueries.WNF_TMCN_ISTABLETMODE, wnfCallback);
            WnfQueries.SubscribeWnf(WnfQueries.WNF_CELL_SIGNAL_STRENGTH_BARS_CAN0, wnfCallback);
            WnfQueries.SubscribeWnf(WnfQueries.WNF_CELL_SIGNAL_STRENGTH_BARS_CAN1, wnfCallback);
        }

        private async Task MonitorPhoneAsync()
        {
            var phoneLines = await GetPhoneLinesAsync();
            var isDualSim = phoneLines.Count > 1;

            //PhoneCallManager.CallStateChanged += PhoneCallManager_CallStateChanged;

            foreach (var phoneLine in phoneLines)
            {
                stBar.AddPhoneLine(phoneLine, isDualSim);
                //phoneLine.LineChanged += PhoneLine_LineChanged;
            }           
        }

        private void PhoneCallManager_CallStateChanged(object sender, object e)
        {
            //throw new NotImplementedException();
            
        }

        private void PhoneLine_LineChanged(PhoneLine sender, object args)
        {
            
        }

        private async Task<List<PhoneLine>> GetPhoneLinesAsync()
        {
            PhoneCallStore store = await PhoneCallManager.RequestStoreAsync();
            var watcher = store.RequestLineWatcher();
            var phoneLines = new List<PhoneLine>();
            var lineEnumerationCompletion = new TaskCompletionSource<bool>();

            watcher.LineAdded += async (o, args) => { var line = await PhoneLine.FromIdAsync(args.LineId); phoneLines.Add(line); };
            watcher.Stopped += (o, args) => lineEnumerationCompletion.TrySetResult(false);
            watcher.EnumerationCompleted += (o, args) => lineEnumerationCompletion.TrySetResult(true);

            watcher.Start();

            if (!await lineEnumerationCompletion.Task)
                throw new Exception("Phone Line Enumeration failed");

            watcher.Stop();

            List<PhoneLine> returnedLines = new List<PhoneLine>();

            foreach (PhoneLine phoneLine in phoneLines)
                if (phoneLine != null && phoneLine.Transport == PhoneLineTransport.Cellular)
                    returnedLines.Add(phoneLine);

            return returnedLines;
        }

        public static void ResetWorkArea()
        {
            RECT oldWorkArea;
            oldWorkArea.Left = Screen.PrimaryScreen.Bounds.Left;
            oldWorkArea.Top = Screen.PrimaryScreen.Bounds.Top;
            oldWorkArea.Right = Screen.PrimaryScreen.Bounds.Width;
            oldWorkArea.Bottom = Screen.PrimaryScreen.Bounds.Height;

            SystemParametersInfo(SPI_SETWORKAREA, 0, ref oldWorkArea, 1 | 2);
        }

        static void Application_ThreadException(object sender, ThreadExceptionEventArgs e)
        {
            MessageBox.Show("TE - RIP: \n" + e.Exception.Message);
        }

        static void CurrentDomain_UnhandledException(object sender, UnhandledExceptionEventArgs e)
        {
            MessageBox.Show("RIP: \n" + e.ExceptionObject);
        }

        private void kbh_OnKeyUnpressed(object sender, VirtualKeyShort e)
        {  }

        private void kbh_OnKeyPressed(object sender, VirtualKeyShort e)
        {
            if (e == VirtualKeyShort.VOLUME_UP || e == VirtualKeyShort.VOLUME_DOWN)
                vlFly.SetVolume(e);
        }

        public static void UpdateScreenAppBar()
        {
            ResetScreenCache();
            stBar.SetupAppBar();
            tkBar.SetupAppBar();
        }

        public static void ResetScreenCache()
        {
            // use reflection to empty screens cache
            typeof(Screen).GetField("screens", System.Reflection.BindingFlags.Static | System.Reflection.BindingFlags.NonPublic).SetValue(null, null);
        }

        public IntPtr WnfStateUpdates(ulong stateName, uint changeStamp, IntPtr typeId, IntPtr callbackContext, IntPtr bufferPtr, uint bufferSize)
        {
            byte[] buffer = new byte[bufferSize];
            Marshal.Copy(bufferPtr, buffer, 0, buffer.Length);

            if (stateName == WnfQueries.WNF_SHEL_QUIETHOURS_ACTIVE_PROFILE_CHANGED)
            {
                var focusAssistStatus = WnfQueries.ToFocusAssistStatus(buffer);
                stBar?.UpdateFocusAssist(focusAssistStatus);
            }
            else if (stateName == WnfQueries.WNF_LFS_STATE)
            {
                //LOCATION
                var locationInUse = WnfQueries.ToBool(buffer);
                stBar?.UpdateLocation(locationInUse);
            }
            else if (stateName == WnfQueries.WNF_SHEL_NOTIFICATIONS)
            {
                //NOTIFICATION
                var notificationsCount = WnfQueries.ToInt32(buffer);
                stBar?.UpdateNotification(notificationsCount);
            }
            else if (stateName == WnfQueries.WNF_TMCN_ISTABLETMODE)
            {
                //TABLET MODE
                var isTabletMode = WnfQueries.ToBool(buffer);
                IsTabletMode = isTabletMode;
                ChangeMobileShellBehaviour(isTabletMode);
            }
            else if (stateName == WnfQueries.WNF_CELL_SIGNAL_STRENGTH_BARS_CAN0)
            {
                var signalStrength = WnfQueries.ToInt32(buffer);
                stBar?.UpdateSignalStrength(signalStrength, 0);
            }
            else if (stateName == WnfQueries.WNF_CELL_SIGNAL_STRENGTH_BARS_CAN1)
            {
                var signalStrength = WnfQueries.ToInt32(buffer);
                stBar?.UpdateSignalStrength(signalStrength, 0);
            }
            return IntPtr.Zero;
        }

        public static void ChangeMobileShellBehaviour(bool visible)
        {
            if (visible)
            {
                stBar?.SetVisibility(Visibility.Visible);
                tkBar?.SetVisibility(Visibility.Visible);
                //stBar.UpdateAppBar(); //STACK OVERFLOW EXCEPTION !!!!111!!!11!
                stBar?.SetupAppBar();
                //tkBar.UpdateAppBar();
                tkBar?.SetupAppBar();
                HideExplorerTaskbar();
            }
            else
            {
                stBar?.SetVisibility(Visibility.Collapsed);
                tkBar?.SetVisibility(Visibility.Collapsed);
                //stBar.UpdateAppBar();
                stBar?.UnSetupAppBar();
                //tkBar.UpdateAppBar();
                tkBar?.UnSetupAppBar();
                ResetWorkArea();
                ShowExplorerTaskbar();
            }
        }

        public static void HideExplorerTaskbar()
        {
            //Enable Autohide.
            AppBar.SetWinTaskbarState(AppBar.WinTaskbarState.AutoHide);

            //puff, disappear.
            AppBar.SetWinTaskbarPos((int)SetWindowPosFlags.SWP_HIDEWINDOW);
        }

        public static void ShowExplorerTaskbar()
        {
            //Reset.
            AppBar.SetWinTaskbarState(AppBar.WinTaskbarState.OnTop);

            //Reset.
            AppBar.SetWinTaskbarPos((int)SetWindowPosFlags.SWP_SHOWWINDOW);
        }
    }
}
