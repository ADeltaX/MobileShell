using MobileShell.Classes;
using System;
using System.Windows;
using System.Windows.Forms;
using System.Windows.Interop;
using System.Windows.Threading;
using static MobileShell.Classes.NativeMethods;

namespace MobileShell
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class StatusBarWindow : Window
    {
        int appbarMessageId = -1;
        bool troll = false;

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

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            //AcrylicBlur ab = new AcrylicBlur(this);
            //ab.EnableBlur();

            WindowInteropHelper wndHelper = new WindowInteropHelper(this);

            int exStyle = (int)GetWindowLong(wndHelper.Handle, (int)GetWindowLongFields.GWL_EXSTYLE);

            exStyle |= (int)ExtendedWindowStyles.WS_EX_TOOLWINDOW;
            SetWindowLong(wndHelper.Handle, (int)GetWindowLongFields.GWL_EXSTYLE, (IntPtr)exStyle);

            appbarMessageId = AppBar.RegisterBar(this, Screen.PrimaryScreen, Width * App.DPI, Height * App.DPI, ABEdge.ABE_TOP);

            App.Configure();

            DispatcherTimer disp = new DispatcherTimer(TimeSpan.FromMilliseconds(500), DispatcherPriority.Background, delegate {
                batteryGlyph.UnicodeString = (troll ? "\uEBB3" : "\uEBA8");
                troll = !troll;
            }, Dispatcher);


            //CLOCK
            DispatcherTimer clock = new DispatcherTimer(TimeSpan.FromMilliseconds(500), DispatcherPriority.Background, delegate
            {
                textBlock_Clock.Text = DateTime.Now.ToString("HH:mm");
            }, Dispatcher);

        }
    }
}
