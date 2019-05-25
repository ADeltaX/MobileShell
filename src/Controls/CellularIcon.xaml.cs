using MobileShell.Enums;
using System;
using System.Windows;
using System.Windows.Controls;
using Windows.ApplicationModel.Calls;

namespace MobileShell.Controls
{
    /// <summary>
    /// Interaction logic for CellularIcon.xaml
    /// </summary>
    public partial class CellularIcon : ContentControl
    {
        public Guid PhoneGuid { get; set; }

        public CellularIcon()
        {
            InitializeComponent();
        }

        public bool IsDataCellularEnabled
        {
            get { return (bool)GetValue(IsDataCellularEnabledProperty); }
            set { SetValue(IsDataCellularEnabledProperty, value); }
        }

        public static readonly DependencyProperty IsDataCellularEnabledProperty =
            DependencyProperty.Register("IsDataCellularEnabled", typeof(bool), typeof(CellularIcon), new PropertyMetadata(false));


        public PhoneNetworkState PhoneNetworkState
        {
            get { return (PhoneNetworkState)GetValue(PhoneNetworkStateProperty); }
            set { SetValue(PhoneNetworkStateProperty, value); }
        }

        public static readonly DependencyProperty PhoneNetworkStateProperty =
            DependencyProperty.Register("PhoneNetworkState", typeof(PhoneNetworkState), typeof(CellularIcon), new PropertyMetadata(PhoneNetworkState.NoSignal));


        public bool IsDualSim
        {
            get { return (bool)GetValue(IsDualSimProperty); }
            set { SetValue(IsDualSimProperty, value); }
        }

        public static readonly DependencyProperty IsDualSimProperty =
            DependencyProperty.Register("IsDualSim", typeof(bool), typeof(CellularIcon), new PropertyMetadata(false));


        public int DualSimStatusNumber
        {
            get { return (int)GetValue(DualSimStatusNumberProperty); }
            set { SetValue(DualSimStatusNumberProperty, value); }
        }

        public static readonly DependencyProperty DualSimStatusNumberProperty =
            DependencyProperty.Register("DualSimStatusNumber", typeof(int), typeof(CellularIcon), new PropertyMetadata(1));


        public PhoneSimState PhoneSimState
        {
            get { return (PhoneSimState)GetValue(PhoneSimStateProperty); }
            set { SetValue(PhoneSimStateProperty, value); }
        }

        public static readonly DependencyProperty PhoneSimStateProperty =
            DependencyProperty.Register("PhoneSimState", typeof(PhoneSimState), typeof(CellularIcon), new PropertyMetadata(PhoneSimState.Unknown));


        public SignalStrength SignalStrength
        {
            get { return (SignalStrength)GetValue(SignalStrengthProperty); }
            set { SetValue(SignalStrengthProperty, value); }
        }

        public static readonly DependencyProperty SignalStrengthProperty =
            DependencyProperty.Register("SignalStrength", typeof(SignalStrength), typeof(CellularIcon), new PropertyMetadata(SignalStrength.Strength_0));
    }
}
