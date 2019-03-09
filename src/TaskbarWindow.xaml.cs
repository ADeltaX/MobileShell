using MobileShell.Classes;
using System;
using System.Diagnostics;
using System.Windows;
using System.Windows.Forms;
using System.Windows.Input;
using System.Windows.Interop;
using static MobileShell.Classes.NativeMethods;
namespace MobileShell
{
    /// <summary>
    /// Interaction logic for TaskbarWindow.xaml
    /// </summary>
    public partial class TaskbarWindow : Window
    {
        int appbarMessageId = -1;
        private readonly Stopwatch _doubleTapStopwatch = new Stopwatch();
        private Point? _lastTapLocation;

        public TaskbarWindow()
        {
            ShowActivated = false;
            ShowInTaskbar = false;
            Loaded += Window_Loaded;
            InitializeComponent();

            Width = SystemParameters.PrimaryScreenWidth;
            Top = SystemParameters.PrimaryScreenHeight - Height;
            Left = 0;
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
                if (msg == 0x0006)
                    AppBar.AppBarActivate(hwnd);
                else
                    App.HideExplorerTaskbar();

                return IntPtr.Zero;
            }
            else if (msg == WM_MOUSEACTIVATE)
            {
                handled = true;
                return new IntPtr(MA_NOACTIVATE);
            }
            else
            {
                return IntPtr.Zero;
            }
        }

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            //AcrylicBlur ab = new AcrylicBlur(this);
            //ab.EnableBlur();

            WindowInteropHelper wndHelper = new WindowInteropHelper(this);

            int exStyle = (int)GetWindowLong(wndHelper.Handle, (int)GetWindowLongFields.GWL_EXSTYLE);

            exStyle |= (int)ExtendedWindowStyles.WS_EX_TOOLWINDOW | (int)ExtendedWindowStyles.WS_EX_NOACTIVATE;
            SetWindowLong(wndHelper.Handle, (int)GetWindowLongFields.GWL_EXSTYLE, (IntPtr)exStyle);

            SetWindowPos(wndHelper.Handle, HWND_TOPMOST, 0, 0, 0, 0, TOPMOST_FLAGS);

            appbarMessageId = AppBar.RegisterBar(this, Screen.PrimaryScreen, Width * App.DPI, Height * App.DPI, ABEdge.ABE_BOTTOM);

            App.Configure();
        }

        private void Grid_PreviewTouchDown(object sender, System.Windows.Input.TouchEventArgs e)
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
            if (await WPFHelper.TouchHold(sender as FrameworkElement, TimeSpan.FromSeconds(.5))) //More than enough
            {
                ShellKeyCombo(VirtualKeyShort.LWIN, VirtualKeyShort.TAB);
            }
            else
            {
                //TODO: Desktop mode uses ALT + ESC, so check if it's in tablet mode or desktop mode
                ShellKeyCombo(VirtualKeyShort.LWIN, VirtualKeyShort.BACK);
            }
        }
    }
}
