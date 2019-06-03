using AudioSwitcher.AudioApi;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MobileShell.AudioObservers
{
    internal class DevicePeakValueChangedObserver : IObserver<DevicePeakValueChangedArgs>
    {
        public void OnCompleted() { }

        public void OnError(Exception error) { }

        public void OnNext(DevicePeakValueChangedArgs value)
        {
            VolumeAudioFlyout.CurrentVolumeAudioFlyout.MediaAppsPeak = value.PeakValue / 100;
        }
    }
}
