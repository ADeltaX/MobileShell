using MobileShell.Enums;
using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Data;

namespace MobileShell.Converters
{
    class SignalStrengthToUnicodeConverter : IValueConverter
    {
        private const string EMPTY = "\u000D";
        private const string SIGNAL_STRENGTH_1 = "\uEC37";
        private const string SIGNAL_STRENGTH_2 = "\uEC38";
        private const string SIGNAL_STRENGTH_3 = "\uEC39";
        private const string SIGNAL_STRENGTH_4 = "\uEC3A";
        private const string SIGNAL_STRENGTH_5 = "\uEC3B";

        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            SignalStrength strength = (SignalStrength)value;

            switch (strength)
            {
                case SignalStrength.Strength_0:
                    return EMPTY;
                case SignalStrength.Strength_1:
                    return SIGNAL_STRENGTH_1;
                case SignalStrength.Strength_2:
                    return SIGNAL_STRENGTH_2;
                case SignalStrength.Strength_3:
                    return SIGNAL_STRENGTH_3;
                case SignalStrength.Strength_4:
                    return SIGNAL_STRENGTH_4;
                case SignalStrength.Strength_5:
                    return SIGNAL_STRENGTH_5;
                default:
                    return EMPTY;
            }
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }
}
