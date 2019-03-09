using System;
using System.Diagnostics;
using System.Runtime.InteropServices;
using static MobileShell.Classes.NativeMethods;

namespace MobileShell.Classes
{
    public class HookEngine
    {
        public event EventHandler<VirtualKeyShort> OnKeyPressed;
        public event EventHandler<VirtualKeyShort> OnKeyUnpressed;

        private LowLevelKeyboardProc _proc;
        private IntPtr _hookID = IntPtr.Zero;

        public HookEngine() => _proc = HookCallback;

        public void HookKeyboard() => _hookID = SetHook(_proc);

        public void UnHookKeyboard() => UnhookWindowsHookEx(_hookID);

        private IntPtr SetHook(LowLevelKeyboardProc proc)
        {
            using (Process curProcess = Process.GetCurrentProcess())
                using (ProcessModule curModule = curProcess.MainModule)
                    return SetWindowsHookEx(WH_KEYBOARD_LL, proc, GetModuleHandle(curModule.ModuleName), 0);

        }

        private IntPtr HookCallback(int nCode, IntPtr wParam, IntPtr lParam)
        {
            //TODO: remove the "hard" return when volumedown/up is pressed

            if (nCode >= 0 && wParam == (IntPtr)WM_KEYDOWN || wParam == (IntPtr)WM_SYSKEYDOWN)
            {
                int vkCode = Marshal.ReadInt32(lParam);

                OnKeyPressed?.Invoke(this, (VirtualKeyShort)vkCode);

                if ((VirtualKeyShort)vkCode == VirtualKeyShort.VOLUME_DOWN || (VirtualKeyShort)vkCode == VirtualKeyShort.VOLUME_UP)
                    return (IntPtr)1;

            }
            else if (nCode >= 0 && wParam == (IntPtr)WM_KEYUP || wParam == (IntPtr)WM_SYSKEYUP)
            {
                int vkCode = Marshal.ReadInt32(lParam);

                OnKeyUnpressed?.Invoke(this, (VirtualKeyShort)vkCode);

                if ((VirtualKeyShort)vkCode == VirtualKeyShort.VOLUME_DOWN || (VirtualKeyShort)vkCode == VirtualKeyShort.VOLUME_UP)
                    return (IntPtr)1;
            }

            return CallNextHookEx(_hookID, nCode, wParam, lParam);
        }
    }
}
