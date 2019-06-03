using AudioSwitcher.AudioApi.Session;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MobileShell.AudioObservers
{
    internal class SystemAudioSessionPeakValueChangedObserver : IObserver<SessionPeakValueChangedArgs>
    {
        public void OnCompleted() { }

        public void OnError(Exception error) { }

        public void OnNext(SessionPeakValueChangedArgs value)
        {
            VolumeAudioFlyout.CurrentVolumeAudioFlyout.RingerNotificationsPeak = value.PeakValue / 100;
        }
    }
}
