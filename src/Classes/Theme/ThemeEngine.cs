using Microsoft.Win32;
using System;
using System.Runtime.InteropServices;
using System.Windows;
using System.Windows.Media;
using static MobileShell.Classes.NativeMethods;
namespace MobileShell.Classes
{
    internal class ThemeEngine
    {
        private BaseWindow _msgW;

        public ThemeEngine()
        {
            _msgW = new BaseWindow();
            _msgW.Initialize((m) => WndProc(m.Msg, m.WParam, m.LParam));

            ThemeChanged();
        }

        private static void TrySetColor(string name, string resourceName)
        {
            var color = default(Color);
            var colorSet = GetImmersiveUserColorSetPreference(false, false);
            var colorType = GetImmersiveColorTypeFromName(name);
            var rawColor = GetImmersiveColorFromColorSetEx(colorSet, colorType, false, 0);

            color = ToABGRColor(rawColor);

            SharedResourceDictionary dynamicAccent = Application.Current.Resources.MergedDictionaries[0] as SharedResourceDictionary;

            if (rawColor != 4294902015 && dynamicAccent.Contains(resourceName))
                dynamicAccent[resourceName] = new SolidColorBrush(color);
        }

        public static Color ToABGRColor(uint abgrValue)
        {
            var colorBytes = new byte[4];
            colorBytes[0] = (byte)((0xFF000000 & abgrValue) >> 24);     // A
            colorBytes[1] = (byte)((0x00FF0000 & abgrValue) >> 16);     // B
            colorBytes[2] = (byte)((0x0000FF00 & abgrValue) >> 8);      // G
            colorBytes[3] = (byte)(0x000000FF & abgrValue);             // R

            return Color.FromArgb(colorBytes[0], colorBytes[3], colorBytes[2], colorBytes[1]);
        }

        private void WndProc(int msg, IntPtr wParam, IntPtr lParam)
        {
            switch (msg)
            {
                case WM_DWMCOLORIZATIONCOLORCHANGED:
                case WM_DWMCOMPOSITIONCHANGED:
                case WM_THEMECHANGED:
                    ThemeChanged();
                    break;
                case WM_SETTINGCHANGE:
                    var settingChanged = Marshal.PtrToStringUni(lParam);
                    if (settingChanged == "ImmersiveColorSet" ||    // Accent color
                        settingChanged == "WindowsThemeElement")    // High contrast
                    {
                        ThemeChanged();
                    }
                    break;
            }
        }

        private bool IsLightTheme
        {
            get
            {
                using (RegistryKey key = Registry.CurrentUser.OpenSubKey(@"Software\Microsoft\Windows\CurrentVersion\Themes\Personalize"))
                {
                    object valObj = key?.GetValue("SystemUsesLightTheme");
                    if (valObj == null)
                        return true;
                    else
                        return (int)valObj > 0 ? true : false;
                }
            }
        }

        private void SwitchToLightMode()
        {
            Application.Current.Resources.MergedDictionaries[1].Source = new Uri("/Themes/Light.xaml", UriKind.Relative);
        }

        private void SwitchToDarkMode()
        {
            Application.Current.Resources.MergedDictionaries[1].Source = new Uri("/Themes/Dark.xaml", UriKind.Relative);
        }

        private void ThemeChanged()
        {
            if (IsLightTheme)
                SwitchToLightMode();
            else
                SwitchToDarkMode();

            TrySetColor("ImmersiveSystemAccentLight3", "ImmersiveSystemAccentLight3Brush");
            TrySetColor("ImmersiveSystemAccentLight2", "ImmersiveSystemAccentLight2Brush");
            TrySetColor("ImmersiveSystemAccentLight1", "ImmersiveSystemAccentLight1Brush");
            TrySetColor("ImmersiveSystemAccent", "ImmersiveSystemAccentBrush");
            TrySetColor("ImmersiveSystemAccentDark1", "ImmersiveSystemAccentDark1Brush");
            TrySetColor("ImmersiveSystemAccentDark2", "ImmersiveSystemAccentDark2Brush");
            TrySetColor("ImmersiveSystemAccentDark3", "ImmersiveSystemAccentDark3Brush");
        }
    }
}
