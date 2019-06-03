using AudioSwitcher.AudioApi;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MobileShell.AudioObservers
{
    internal class DeviceChangedObserver : IObserver<DeviceChangedArgs>
    {
        public void OnCompleted() { }

        public void OnError(Exception error) { }

        public void OnNext(DeviceChangedArgs value)
        {
            
        }
    }
}
