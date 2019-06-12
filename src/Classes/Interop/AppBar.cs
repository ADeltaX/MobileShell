using System;
using System.Runtime.InteropServices;
using System.Windows;
using System.Windows.Forms;
using System.Windows.Interop;
using System.Windows.Threading;
using static MobileShell.Classes.NativeMethods;

namespace MobileShell.Classes
{
    public static class AppBar
    {
        private static int uCallBack = 0;
        private static object appBarLock = new object();
        private delegate void ResizeDelegate(IntPtr hWnd, int x, int y, int cx, int cy);

        public enum WinTaskbarState : int
        {
            AutoHide = 1,
            OnTop = 0
        }

        public static void SetWinTaskbarState(WinTaskbarState state)
        {
            NativeMethods.APPBARDATA abd = new NativeMethods.APPBARDATA();
            abd.cbSize = Marshal.SizeOf(typeof(NativeMethods.APPBARDATA));
            abd.hWnd = NativeMethods.FindWindow("Shell_TrayWnd");

            abd.lParam = (IntPtr)state;
            NativeMethods.SHAppBarMessage((int)NativeMethods.ABMsg.ABM_SETSTATE, ref abd);
        }

        public static void SetWinTaskbarPos(int swp)
        {
            IntPtr taskbarInsertAfter = (IntPtr)1;

            IntPtr taskbarHwnd = NativeMethods.FindWindow("Shell_TrayWnd", "");
            IntPtr startButtonHwnd = NativeMethods.FindWindowEx(IntPtr.Zero, IntPtr.Zero, (IntPtr)0xC017, null);

            NativeMethods.SetWindowPos(taskbarHwnd, taskbarInsertAfter, 0, 0, 0, 0, swp | (int)NativeMethods.SetWindowPosFlags.SWP_NOMOVE | (int)NativeMethods.SetWindowPosFlags.SWP_NOSIZE | (int)NativeMethods.SetWindowPosFlags.SWP_NOACTIVATE);
            NativeMethods.SetWindowPos(startButtonHwnd, taskbarInsertAfter, 0, 0, 0, 0, swp | (int)NativeMethods.SetWindowPosFlags.SWP_NOMOVE | (int)NativeMethods.SetWindowPosFlags.SWP_NOSIZE | (int)NativeMethods.SetWindowPosFlags.SWP_NOACTIVATE);
        }

        public static int RegisterBar(Window abWindow, Screen screen, double width, double height, ABEdge edge = ABEdge.ABE_TOP)
        {
            lock (appBarLock)
            {
                APPBARDATA abd = new APPBARDATA();
                abd.cbSize = Marshal.SizeOf(typeof(APPBARDATA));
                IntPtr handle = new WindowInteropHelper(abWindow).Handle;
                abd.hWnd = handle;

                uCallBack = RegisterWindowMessage("AppBarMessage");
                abd.uCallbackMessage = uCallBack;

                uint ret = SHAppBarMessage((int)ABMsg.ABM_NEW, ref abd);

                ABSetPos(abWindow, screen, width, height, edge);
            }

            return uCallBack;
        }

        public static void ABSetPos(Window abWindow, Screen screen, double width, double height, ABEdge edge)
        {
            lock (appBarLock)
            {
                APPBARDATA abd = new APPBARDATA();
                abd.cbSize = Marshal.SizeOf(typeof(APPBARDATA));
                IntPtr handle = new WindowInteropHelper(abWindow).Handle;
                abd.hWnd = handle;
                abd.uEdge = (int)edge;
                int sWidth = (int)width;
                int sHeight = (int)height;

                int top = 0;
                int left = SystemInformation.PrimaryMonitorSize.Width; //TODO
                int right = SystemInformation.PrimaryMonitorSize.Height; //TODO
                int bottom = (int)Math.Floor(42 * App.DPI); //BIGTODO TODO

                if (screen != null)
                {
                    top = screen.Bounds.Y;
                    left = screen.Bounds.Left;
                    right = screen.Bounds.Right;
                    bottom = screen.Bounds.Bottom;
                }

                if (abd.uEdge == (int)ABEdge.ABE_LEFT || abd.uEdge == (int)ABEdge.ABE_RIGHT)
                {
                    abd.rc.Top = top;
                    abd.rc.Bottom = bottom;
                    if (abd.uEdge == (int)ABEdge.ABE_LEFT)
                    {
                        abd.rc.Left = left;
                        abd.rc.Right = abd.rc.Left + sWidth;
                    }
                    else
                    {
                        abd.rc.Right = right;
                        abd.rc.Left = abd.rc.Right - sWidth;
                    }

                }
                else
                {
                    abd.rc.Left = left;
                    abd.rc.Right = right;
                    if (abd.uEdge == (int)ABEdge.ABE_TOP)
                    {
                        if (abWindow is NavbarWindow)
                            abd.rc.Top = top + Convert.ToInt32(24 * App.DPI); //BIGTODO TODO
                        else
                            abd.rc.Top = top;
                        abd.rc.Bottom = abd.rc.Top + sHeight;
                    }
                    else
                    {
                        abd.rc.Bottom = bottom;
                        abd.rc.Top = abd.rc.Bottom - sHeight;
                    }
                }

                SHAppBarMessage((int)ABMsg.ABM_SETPOS, ref abd);

                int h = abd.rc.Bottom - abd.rc.Top;

                abWindow.Dispatcher.BeginInvoke(DispatcherPriority.ApplicationIdle,
                    new ResizeDelegate(DoResize), abd.hWnd, abd.rc.Left, abd.rc.Top, 
                    abd.rc.Right - abd.rc.Left, abd.rc.Bottom - abd.rc.Top);

                if (h < sHeight)
                    ABSetPos(abWindow, screen, width, height, edge);
            }
        }

        public static void AppBarActivate(IntPtr hwnd)
        {
            NativeMethods.APPBARDATA abd = new NativeMethods.APPBARDATA();
            abd.cbSize = (int)Marshal.SizeOf(typeof(NativeMethods.APPBARDATA));
            abd.hWnd = hwnd;
            abd.lParam = (IntPtr)Convert.ToInt32(true);
            NativeMethods.SHAppBarMessage((int)NativeMethods.ABMsg.ABM_ACTIVATE, ref abd);
        }

        private static void DoResize(IntPtr hWnd, int x, int y, int cx, int cy)
        {
            MoveWindow(hWnd, x, y, cx, cy, true);
            SetWinTaskbarPos((int)SetWindowPosFlags.SWP_HIDEWINDOW);
        }
    }
}
