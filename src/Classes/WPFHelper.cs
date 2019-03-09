using System;
using System.Diagnostics;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Input;
using System.Windows.Threading;

namespace MobileShell.Classes
{
    public static class WPFHelper
    {
        private const int MAX_LENGTH = 15;
        private const int MAX_TIME_MILLISECONDS = 250;

        public static bool IsDoubleTap(this TouchEventArgs e, IInputElement inputReference, ref Point? lastTapLocation, Stopwatch doubleTapStopwatch)
        {
            Point currentTapPosition = e.GetTouchPoint(inputReference).Position;
            bool tapsAreCloseInDistance = false;

            if (lastTapLocation != null)
                tapsAreCloseInDistance = Point.Subtract(currentTapPosition, (Point)lastTapLocation).LengthSquared < (MAX_LENGTH * App.DPI);

            lastTapLocation = currentTapPosition;

            TimeSpan elapsed = doubleTapStopwatch.Elapsed;
            doubleTapStopwatch.Restart();
            bool tapsAreCloseInTime = (elapsed != TimeSpan.Zero && elapsed.Milliseconds < MAX_TIME_MILLISECONDS);

            if (tapsAreCloseInTime && tapsAreCloseInDistance)
                lastTapLocation = null;

            return tapsAreCloseInDistance && tapsAreCloseInTime;
        }

        public static Task<bool> TouchHold(FrameworkElement element, TimeSpan duration)
        {
            DispatcherTimer timer = new DispatcherTimer();
            TaskCompletionSource<bool> task = new TaskCompletionSource<bool>();
            timer.Interval = duration;

            MouseButtonEventHandler touchUpHandler = delegate
            {
                timer.Stop();

                if (task.Task.Status != TaskStatus.RanToCompletion)
                    task.SetResult(false);

            };

            element.PreviewMouseUp += touchUpHandler;

            timer.Tick += delegate
            {
                element.PreviewMouseUp -= touchUpHandler;
                timer.Stop();
                task.SetResult(true);
            };

            timer.Start();
            return task.Task;
        }
    }
}
