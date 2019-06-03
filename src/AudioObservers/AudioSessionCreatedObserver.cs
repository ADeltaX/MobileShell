using AudioSwitcher.AudioApi.Session;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MobileShell.AudioObservers
{
    internal class AudioSessionCreatedObserver : IObserver<IAudioSession>
    {
        public void OnCompleted() { }

        public void OnError(Exception error) { }

        public void OnNext(IAudioSession value)
        {
            value.Volume = VolumeAudioFlyout.CurrentVolumeAudioFlyout.MediaAppsVolume;
        }
    }
}
