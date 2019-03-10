using MobileShell.Classes;
using System;
using System.Windows;
using System.Windows.Interop;
using System.Windows.Threading;
using static MobileShell.Classes.NativeMethods;

namespace MobileShell
{
    /// <summary>
    /// Interaction logic for VolumeAudioFlyout.xaml
    /// </summary>
    public partial class VolumeAudioFlyout : Window
    {
        //TODO: INCOMPLETE

        private readonly DispatcherTimer _elapsedTimer;
        private bool isVisible = false;

        public VolumeAudioFlyout()
        {
            InitializeComponent();

            Top = 24F * App.DPI;
            Left = 0;

            Width = SystemParameters.PrimaryScreenWidth;

            Hide();

            _elapsedTimer = new DispatcherTimer { Interval = TimeSpan.FromSeconds(1.5) };
            _elapsedTimer.Tick += (_, __) => 
            {
                _elapsedTimer.Stop();
                Hide();
            };
        }

        public new void Show()
        {
            base.Show();
            _elapsedTimer.Start();
            isVisible = true;
        }

        public new void Hide()
        {
            base.Hide();
            isVisible = false;
        }

        public void SetVolume(NativeMethods.VirtualKeyShort key)
        {
            if (isVisible)
            {
                _elapsedTimer.Stop();
                _elapsedTimer.Start();
            }
            else
            {
                Show();
            }

            if (key == NativeMethods.VirtualKeyShort.VOLUME_DOWN)
                VolDown();
            else if (key == NativeMethods.VirtualKeyShort.VOLUME_UP)
                VolUp();
        }

        private void VolUp()
        {
            if (sldr.Value < sldr.Maximum)
                sldr.Value += 1;
        }

        private void VolDown()
        {
            if (sldr.Value > sldr.Minimum)
                sldr.Value -= 1;
        }

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            WindowInteropHelper wndHelper = new WindowInteropHelper(this);

            int exStyle = (int)GetWindowLong(wndHelper.Handle, (int)GetWindowLongFields.GWL_EXSTYLE);

            exStyle |= (int)ExtendedWindowStyles.WS_EX_TOOLWINDOW | (int)ExtendedWindowStyles.WS_EX_NOACTIVATE;
            SetWindowLong(wndHelper.Handle, (int)GetWindowLongFields.GWL_EXSTYLE, (IntPtr)exStyle);
        }
    }
}
