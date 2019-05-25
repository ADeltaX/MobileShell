using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Data;
using Windows.ApplicationModel.Calls;

namespace MobileShell.Converters
{
    public class PhoneSimToVisibilityConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            PhoneSimState phoneSimState = (PhoneSimState)value;
            bool invert = bool.Parse((string)parameter); //if true => don't show signal and data class
            switch (phoneSimState)
            {
                case PhoneSimState.Unknown:
                    return invert ? Visibility.Visible : Visibility.Hidden;
                case PhoneSimState.PinNotRequired:
                    return invert ? Visibility.Hidden : Visibility.Visible;
                case PhoneSimState.PinUnlocked:
                    return invert ? Visibility.Hidden : Visibility.Visible;
                case PhoneSimState.PinLocked:
                    return invert ? Visibility.Visible : Visibility.Hidden;
                case PhoneSimState.PukLocked:
                    return invert ? Visibility.Visible : Visibility.Hidden;
                case PhoneSimState.NotInserted:
                    return invert ? Visibility.Visible : Visibility.Hidden;
                case PhoneSimState.Invalid:
                    return invert ? Visibility.Visible : Visibility.Hidden;
                case PhoneSimState.Disabled:
                    return invert ? Visibility.Visible : Visibility.Hidden;
                default:
                    return invert ? Visibility.Visible : Visibility.Hidden;
            }
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }
}
