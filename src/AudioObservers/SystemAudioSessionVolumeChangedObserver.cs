using AudioSwitcher.AudioApi.Session;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MobileShell.AudioObservers
{
    internal class SystemAudioSessionVolumeChangedObserver : IObserver<SessionVolumeChangedArgs>
    {
        public void OnCompleted() { }

        public void OnError(Exception error) { }

        public void OnNext(SessionVolumeChangedArgs value)
        {
            VolumeAudioFlyout.CurrentVolumeAudioFlyout.RingerNotificationsVolume = (value.Volume / value.Session.Device.Volume) * 100;
        }
    }
}
