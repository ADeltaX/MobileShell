using System;
using System.Globalization;
using System.Windows.Data;

namespace MobileShell.Converters
{
    public class DualSimStatusNumberToUnicodeConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            int simIndex = (int)value;
            if (simIndex == 0)
                return "\uE884";
            else if (simIndex == 1)
                return "\uE882";
            else
                return "\u0020";
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }
}
