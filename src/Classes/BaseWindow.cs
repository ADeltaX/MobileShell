using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace MobileShell.Classes
{
    public class BaseWindow : NativeWindow, IDisposable
    {
        Action<Message> _wndProc;

        public void Initialize(Action<Message> wndProc)
        {
            _wndProc = wndProc;
            CreateHandle(new CreateParams());
        }

        protected override void WndProc(ref Message m)
        {
            _wndProc(m);
            base.WndProc(ref m);
        }

        public void Dispose() => DestroyHandle();
    }
}
