using MobileShell.Classes;
using System;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Windows;
using System.Windows.Forms;
using System.Windows.Input;
using System.Windows.Interop;
using System.Windows.Media;
using System.Windows.Threading;
using static MobileShell.Classes.NativeMethods;
using MessageBox = System.Windows.MessageBox;

namespace MobileShell
{
    /// <summary>
    /// Interaction logic for TaskbarWindow.xaml
    /// </summary>
    public partial class NavbarWindow : Window
    {
        int appbarMessageId = -1;
        private readonly Stopwatch _doubleTapStopwatch = new Stopwatch();
        private Point? _lastTapLocation;

        public NavbarWindow()
        {
            ShowActivated = false;
            ShowInTaskbar = false;
            Loaded += Window_Loaded;
            InitializeComponent();

            Width = SystemParameters.PrimaryScreenWidth;
            Top = SystemParameters.PrimaryScreenHeight - Height;
            Left = 0;
        }

        public void SetVisibility(Visibility vis) => Dispatcher.BeginInvoke(DispatcherPriority.Send, new Action(() => Visibility = vis));

        public void SetupAppBar()
        {
            Dispatcher.BeginInvoke(DispatcherPriority.Send, new Action(() =>
            {
                WindowInteropHelper wndHelper = new WindowInteropHelper(this);

                int exStyle = (int)GetWindowLong(wndHelper.Handle, (int)GetWindowLongFields.GWL_EXSTYLE);

                exStyle |= (int)ExtendedWindowStyles.WS_EX_TOOLWINDOW | (int)ExtendedWindowStyles.WS_EX_NOACTIVATE;
                SetWindowLong(wndHelper.Handle, (int)GetWindowLongFields.GWL_EXSTYLE, (IntPtr)exStyle);

                SetWindowPos(wndHelper.Handle, HWND_TOPMOST, 0, 0, 0, 0, TOPMOST_FLAGS);

                appbarMessageId = AppBar.RegisterBar(this, Screen.PrimaryScreen, Width * App.DPI, Height * App.DPI, ABEdge.ABE_BOTTOM); //Height
                //Turn();
            }));
        }

        public void UpdateAppBar()
        {
            Dispatcher.BeginInvoke(DispatcherPriority.Send, new Action(() => 
                AppBar.ABSetPos(this, Screen.PrimaryScreen, Width * App.DPI, Height * App.DPI, ABEdge.ABE_BOTTOM)));
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

        //Todo
        private void Turn()
        {
            appbarMessageId = AppBar.RegisterBar(this, Screen.PrimaryScreen, 48 * App.DPI, Screen.PrimaryScreen.Bounds.Height / App.DPI, ABEdge.ABE_RIGHT);

            gridTaskBar.RenderTransformOrigin = new Point(0.5, 0.5);
            gridTaskBar.LayoutTransform = new RotateTransform(-90);

            searchButton.RenderTransformOrigin = new Point(0.5, 0.5);
            searchButton.LayoutTransform = new RotateTransform(90);

            windowsButton.RenderTransformOrigin = new Point(0.5, 0.5);
            windowsButton.LayoutTransform = new RotateTransform(90);

            backButton.RenderTransformOrigin = new Point(0.5, 0.5);
            backButton.LayoutTransform = new RotateTransform(90);
        }

        protected override void OnSourceInitialized(EventArgs e)
        {
            base.OnSourceInitialized(e);
            var source = PresentationSource.FromVisual(this) as HwndSource;
            source.AddHook(WndProc);
        }

        private IntPtr WndProc(IntPtr hwnd, int msg, IntPtr wParam, IntPtr lParam, ref bool handled)
        {
            if (msg == appbarMessageId && appbarMessageId != -1)
            {
                if (App.IsTabletMode)
                {
                    if (msg == 0x0006)
                        AppBar.AppBarActivate(hwnd);
                    else
                        App.HideExplorerTaskbar();
                }

                return IntPtr.Zero;
            }
            else if (msg == WM_MOUSEACTIVATE)
            {
                handled = true;
                return new IntPtr(MA_NOACTIVATE);
            }
            else if (msg == WM_DISPLAYCHANGE)
            {
                //MessageBox.Show("WM_DISPLAYCHANGE");
                App.UpdateScreenAppBar();
            }
            else if (msg == WM_DPICHANGED)
            {
                //MessageBox.Show("WM_DPICHANGED");
                App.DPI = (wParam.ToInt32() & 0xFFFF) / 96F;
                App.UpdateScreenAppBar();
            }

            return IntPtr.Zero;
        }

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            //new AcrylicBlur(this).EnableBlur();

            //Turn();
        }

        private void Grid_PreviewTouchDown(object sender, TouchEventArgs e)
        {
            if (e.IsDoubleTap(this, ref _lastTapLocation, _doubleTapStopwatch))
                SendMessage(0xFFFF, 0x112, 0xF170, 2);

            //SendMessage(HWND_BROADCAST, WM_SYSCOMMAND, SC_MONITORPOWER, 2)
        }

        private void WindowsLogo_Click(object sender, RoutedEventArgs e)
        {
            ShellKey(VirtualKeyShort.LWIN);
        }

        private void SearchButton_Click(object sender, RoutedEventArgs e)
        {
            ShellKeyCombo(VirtualKeyShort.LWIN, VirtualKeyShort.KEY_Q);
        }

        private async void Button_PreviewTouchDown(object sender, TouchEventArgs e)
        {
            if (await WPFHelper.TouchHold(sender as FrameworkElement, TimeSpan.FromSeconds(.10))) //More than enough
            {
                ShellKeyCombo(VirtualKeyShort.LWIN, VirtualKeyShort.TAB);
            }
            else
            {
                //Desktop mode uses ALT + ESC
                ShellKeyCombo(VirtualKeyShort.LWIN, VirtualKeyShort.BACK);
            }
        }

        /// <summary>
        /// Desktop support for task view/timeline
        /// </summary>
        private void BackButton_MouseRightButtonUp(object sender, MouseButtonEventArgs e)
        {
            ShellKeyCombo(VirtualKeyShort.LWIN, VirtualKeyShort.TAB);
        }

        private void WindowsButton_MouseRightButtonUp(object sender, MouseButtonEventArgs e)
        {
            ShellKeyCombo(VirtualKeyShort.LWIN, VirtualKeyShort.KEY_X);
        }
    }
}
