using MobileShell.Classes;
using System;
using System.Threading;
using System.Windows;
using System.Windows.Forms;
using System.Windows.Media;
using static MobileShell.Classes.NativeMethods;

namespace MobileShell
{
    /// <summary>
    /// Interaction logic for App.xaml
    /// </summary>
    public partial class App : System.Windows.Application
    {
        public static float DPI { get; private set; } = 1F;
        public static HookEngine Kbh { get; set; }

        #region Main instances of Window(s)

        private StatusBarWindow stBar;
        private TaskbarWindow tkBar;
        private VolumeAudioFlyout vlFly;

        #endregion

        private void Application_Startup(object sender, StartupEventArgs e)
        {
            //ShowExplorerTaskbar();
            //return;


            AppDomain.CurrentDomain.UnhandledException += CurrentDomain_UnhandledException;

            Kbh = new HookEngine();
            Kbh.OnKeyPressed += kbh_OnKeyPressed;
            Kbh.OnKeyUnpressed += kbh_OnKeyUnpressed;
            Kbh.HookKeyboard();

            
            stBar = new StatusBarWindow();
            tkBar = new TaskbarWindow();
            vlFly = new VolumeAudioFlyout();

            DPI = (float)VisualTreeHelper.GetDpi(stBar).DpiScaleX;

            stBar.Show();
            tkBar.Show();

            Configure();
        }

        static void Application_ThreadException(object sender, ThreadExceptionEventArgs e)
        {
            
        }
        static void CurrentDomain_UnhandledException(object sender, UnhandledExceptionEventArgs e)
        {
            System.Windows.MessageBox.Show("RIP");
        }

        private void kbh_OnKeyUnpressed(object sender, VirtualKeyShort e)
        {  }

        private void kbh_OnKeyPressed(object sender, VirtualKeyShort e)
        {
            if (e == VirtualKeyShort.VOLUME_UP || e == VirtualKeyShort.VOLUME_DOWN)
                vlFly.SetVolume(e);

        }

        public static void Configure()
        {
            //Useless?

            HideExplorerTaskbar();
        }

        public static void HideExplorerTaskbar()
        {
            //Enable Autohide.
            AppBar.SetWinTaskbarState(AppBar.WinTaskbarState.AutoHide);

            //puff, disappear.
            AppBar.SetWinTaskbarPos((int)NativeMethods.SetWindowPosFlags.SWP_HIDEWINDOW);
        }

        public static void ShowExplorerTaskbar()
        {
            //Reset.
            AppBar.SetWinTaskbarState(AppBar.WinTaskbarState.OnTop);

            //Reset.
            AppBar.SetWinTaskbarPos((int)NativeMethods.SetWindowPosFlags.SWP_SHOWWINDOW);
        }

        public static void SetWorkArea(Screen screen)
        {
            RECT rect;
            rect.Left = screen.Bounds.Left;
            rect.Right = screen.Bounds.Right;
            rect.Top = (int)Math.Floor(screen.Bounds.Top + (24F * DPI));
            rect.Bottom = (int)Math.Floor(screen.Bounds.Bottom - (42F * DPI));

            SystemParametersInfo(SPI_SETWORKAREA, 0, ref rect, 0);
        }
    }
}
