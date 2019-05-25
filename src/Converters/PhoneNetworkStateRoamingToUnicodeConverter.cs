using System;
using System.Globalization;
using System.Windows.Data;
using Windows.ApplicationModel.Calls;

namespace MobileShell.Converters
{
    public class PhoneNetworkStateRoamingToUnicodeConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            PhoneNetworkState phoneNetworkState = (PhoneNetworkState)value;
            switch (phoneNetworkState)
            {                   
                case PhoneNetworkState.RoamingInternational:
                    return "\uE878";
                case PhoneNetworkState.RoamingDomestic:
                    return "\uE879";
                default:
                    return "";
            }
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }
}
