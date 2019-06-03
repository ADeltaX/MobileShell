using AudioSwitcher.AudioApi.CoreAudio;
using AudioSwitcher.AudioApi.Session;
using MobileShell.AudioObservers;
using MobileShell.Classes;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
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
        private CoreAudioController audioController;
        private CoreAudioDevice defaultDevice;
        private IAudioSession systemSession;
        private double mediaAppsPeak;
        private double ringerNotificationsPeak;
        private double mediaAppsVolume;
        private double ringerNotificationsVolume;

        public static VolumeAudioFlyout CurrentVolumeAudioFlyout { get; private set; }

        public double MediaAppsPeak
        {
            get => mediaAppsPeak;
            set
            {
                Dispatcher.BeginInvoke(DispatcherPriority.Render, new Action(() => {
                    MediaAppsSlider.PeakValue1 = (float)value;
                    MediaAppsSlider.PeakValue2 = (float)value;
                }));
                mediaAppsPeak = value;
            }
        }

        public double RingerNotificationsPeak
        {
            get => ringerNotificationsPeak;
            set
            {
                Dispatcher.BeginInvoke(DispatcherPriority.Render, new Action(() => {
                    RingerNotificationSlider.PeakValue1 = (float)value;
                    RingerNotificationSlider.PeakValue2 = (float)value;
                }));
                ringerNotificationsPeak = value;
            }
        }

        public double RingerNotificationsVolume
        {
            get => ringerNotificationsVolume;
            set
            {
                //Dispatcher.BeginInvoke(DispatcherPriority.Render, new Action(() => {
                //    RingerNotificationSlider.Value = value / 10;
                //}));
                ringerNotificationsVolume = value / 10; //TODO
            }
        }

        public double MediaAppsVolume
        {
            get => mediaAppsVolume;
            set
            {
                //Dispatcher.BeginInvoke(DispatcherPriority.Render, new Action(() => {
                //    RingerNotificationSlider.Value = value / 10;
                //}));
                mediaAppsVolume = value / 30; //TODO
            }
        }

        private List<IAudioSession> audioSessions;
        

        public VolumeAudioFlyout()
        {
            CurrentVolumeAudioFlyout = this;

            audioSessions = new List<IAudioSession>();

            InitializeComponent();
            var audioController = new CoreAudioController();
            audioController.AudioDeviceChanged.Subscribe(new DeviceChangedObserver());
            var devices = audioController.GetPlaybackDevices(AudioSwitcher.AudioApi.DeviceState.Active).ToList();

            devices.ForEach(device =>
            {
                if (device.IsDefaultDevice)
                {
                    defaultDevice = device;
                    device.PeakValueChanged.Subscribe(new DevicePeakValueChangedObserver());
                    device.SessionController.SessionCreated.Subscribe(new AudioSessionCreatedObserver());
                    device.SessionController.ToList().ForEach(audioSession =>
                    {
                        if (audioSession.IsSystemSession)
                        {
                            systemSession = audioSession;
                            audioSession.VolumeChanged.Subscribe(new SystemAudioSessionVolumeChangedObserver());
                            audioSession.PeakValueChanged.Subscribe(new SystemAudioSessionPeakValueChangedObserver());
                        }
                        else
                            audioSessions.Add(audioSession);
                    });
                }
            });

            defaultDevice.Volume = 100;

            Top = 24F * App.DPI * 0;
            Left = 0;

            Width = SystemParameters.PrimaryScreenWidth;

            Hide();

            _elapsedTimer = new DispatcherTimer { Interval = TimeSpan.FromSeconds(2) }; //TODO: Restore to 0
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

        public void SetVolume(VirtualKeyShort key)
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

            if (key == VirtualKeyShort.VOLUME_DOWN)
                VolDown();
            else if (key == VirtualKeyShort.VOLUME_UP)
                VolUp();
        }

        private void VolUp()
        {
            if (MediaAppsSlider.Value < MediaAppsSlider.Maximum)
                MediaAppsSlider.Value += 1;
        }

        private void VolDown()
        {
            if (MediaAppsSlider.Value > MediaAppsSlider.Minimum)
                MediaAppsSlider.Value -= 1;
        }

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            WindowInteropHelper wndHelper = new WindowInteropHelper(this);

            int exStyle = (int)GetWindowLong(wndHelper.Handle, (int)GetWindowLongFields.GWL_EXSTYLE);

            exStyle |= (int)ExtendedWindowStyles.WS_EX_TOOLWINDOW | (int)ExtendedWindowStyles.WS_EX_NOACTIVATE;
            SetWindowLong(wndHelper.Handle, (int)GetWindowLongFields.GWL_EXSTYLE, (IntPtr)exStyle);
        }

        private void Window_PreviewMouseUp(object sender, MouseButtonEventArgs e)
        {
            _elapsedTimer.Stop();
            _elapsedTimer.Start();
        }

        private void Window_PreviewStylusUp(object sender, StylusEventArgs e)
        {
            _elapsedTimer.Stop();
            _elapsedTimer.Start();
        }

        private void Window_PreviewMouseDown(object sender, MouseButtonEventArgs e)
        {
            _elapsedTimer.Stop();
        }

        private void Window_PreviewStylusDown(object sender, StylusDownEventArgs e)
        {
            _elapsedTimer.Stop();
        }

        private void RingerNotificationSlider_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            var value = Math.Truncate(RingerNotificationSlider.Value);
            if (value < 1)
            {
                if (systemSession != null)
                {
                    systemSession.Volume = 0;
                    systemSession.IsMuted = true;
                }
                RingerGlyph.UnicodeString = "\uE7ED";
            }
            else
            {
                if (systemSession != null)
                {
                    systemSession.Volume = (value / RingerNotificationSlider.Maximum) * 100;
                    systemSession.IsMuted = false;
                }
                RingerGlyph.UnicodeString = "\uEA8F";
            }

            ringerNotificationsVolume = (value / RingerNotificationSlider.Maximum) * 100;
        }

        private void MediaAppsSlider_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            var value = Math.Truncate(MediaAppsSlider.Value);
            if (value < 1)
            {
                VolumeGlyph.UnicodeString = "\uE74F";

                defaultDevice?.SessionController.ToList().ForEach(audioSession =>
                {
                    if (!audioSession.IsSystemSession)
                    {
                        audioSession.Volume = 0;
                        audioSession.IsMuted = true;
                    }
                });

            }
            else if (value < 10)
            {
                VolumeGlyph.UnicodeString = "\uE993";
            }
            else if (value < 20)
            {
                VolumeGlyph.UnicodeString = "\uE994";
            }
            else
            {
                VolumeGlyph.UnicodeString = "\uE995";
            }

            if (value >= 1)
            {
                defaultDevice?.SessionController.ToList().ForEach(audioSession =>
                {
                    if (!audioSession.IsSystemSession)
                        audioSession.Volume = (value / MediaAppsSlider.Maximum) * 100;
                    audioSession.IsMuted = false;
                });
            }

            mediaAppsVolume = (value / MediaAppsSlider.Maximum) * 100;
        }

        private void Window_KeyUp(object sender, KeyEventArgs e)
        {
            //Useless
            if (e.Key == Key.Escape)
            {
                Hide();
            }
        }
    }
}
