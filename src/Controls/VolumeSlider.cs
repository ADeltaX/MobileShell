using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Controls.Primitives;
using System.Windows.Input;

namespace MobileShell.Controls
{
    //Straight from EarTrumpet @ https://github.com/File-New-Project/EarTrumpet
    public class VolumeSlider : Slider
    {


        public float PeakValue1
        {
            get => (float)GetValue(PeakValue1Property);
            set => SetValue(PeakValue1Property, value);
        }
        public static readonly DependencyProperty PeakValue1Property = DependencyProperty.Register(
          "PeakValue1", typeof(float), typeof(VolumeSlider), new PropertyMetadata(0f, new PropertyChangedCallback(PeakValueChanged)));

        public float PeakValue2
        {
            get => (float)GetValue(PeakValue2Property);
            set => SetValue(PeakValue2Property, value);
        }
        public static readonly DependencyProperty PeakValue2Property = DependencyProperty.Register(
          "PeakValue2", typeof(float), typeof(VolumeSlider), new PropertyMetadata(0f, new PropertyChangedCallback(PeakValueChanged)));

        private Border _peakMeter1;
        private Border _peakMeter2;
        private Thumb _thumb;
        private bool _clickedInSlider;

        public VolumeSlider() : base()
        {
            Stylus.SetIsPressAndHoldEnabled(this, false);

            AddHandler(PreviewMouseLeftButtonDownEvent, new RoutedEventHandler(PreviewMouseLeftButtonDownSlider), true);
            AddHandler(PreviewMouseLeftButtonUpEvent, new RoutedEventHandler(PreviewMouseLeftButtonUpSlider), true);

            PreviewTouchDown += OnTouchDown;
            PreviewMouseDown += OnMouseDown;
            TouchUp += OnTouchUp;
            MouseUp += OnMouseUp;
            TouchMove += OnTouchMove;
            MouseMove += OnMouseMove;
            MouseWheel += OnMouseWheel;
            Loaded += OnLoaded;
        }

        private void OnLoaded(object sender, RoutedEventArgs e)
        {
            _thumb = (Thumb)GetTemplateChild("SliderThumb");
            _peakMeter1 = (Border)GetTemplateChild("PeakMeter1");
            _peakMeter2 = (Border)GetTemplateChild("PeakMeter2");
        }

        protected override Size ArrangeOverride(Size arrangeBounds)
        {
            var ret = base.ArrangeOverride(arrangeBounds);
            SizeOrVolumeOrPeakValueChanged();
            return ret;
        }

        private static void PeakValueChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            ((VolumeSlider)d).SizeOrVolumeOrPeakValueChanged();
        }

        public void MouseMoveSlider(object sender, MouseEventArgs args)
        {

        }

        public void PreviewMouseLeftButtonDownSlider(object sender, RoutedEventArgs args) => _clickedInSlider = true;
        public void PreviewMouseLeftButtonUpSlider(object sender, RoutedEventArgs args) => _clickedInSlider = false;

        private void SizeOrVolumeOrPeakValueChanged()
        {
            if (_peakMeter1 != null)
            {
                var pos = _thumb.TranslatePoint(new Point(0, 0), this);
                _peakMeter1.Width = Math.Max(0, pos.X * PeakValue1);
            }

            if (_peakMeter2 != null)
            {
                var pos = _thumb.TranslatePoint(new Point(0, 0), this);
                _peakMeter2.Width = Math.Max(0, pos.X * PeakValue2);
            }
        }

        private void OnTouchDown(object sender, TouchEventArgs e)
        {
            VisualStateManager.GoToState((FrameworkElement)sender, "Pressed", true);

            SetPositionByControlPoint(e.GetTouchPoint(this).Position);
            CaptureTouch(e.TouchDevice);

            e.Handled = true;
        }

        private void OnMouseDown(object sender, MouseButtonEventArgs e)
        {
            if (e.LeftButton == MouseButtonState.Pressed)
            {
                VisualStateManager.GoToState((FrameworkElement)sender, "Pressed", true);

                SetPositionByControlPoint(e.GetPosition(this));
                CaptureMouse();

                e.Handled = true;
            }
        }

        private void OnTouchUp(object sender, TouchEventArgs e)
        {
            VisualStateManager.GoToState((FrameworkElement)sender, "Normal", true);

            ReleaseTouchCapture(e.TouchDevice);
            e.Handled = true;
        }

        private void OnMouseUp(object sender, MouseButtonEventArgs e)
        {
            if (IsMouseCaptured)
            {
                // If the point is outside of the control, clear the hover state.
                Rect rcSlider = new Rect(0, 0, ActualWidth, ActualHeight);
                if (!rcSlider.Contains(e.GetPosition(this)))
                {
                    VisualStateManager.GoToState((FrameworkElement)sender, "Normal", true);
                }

                ReleaseMouseCapture();
                e.Handled = true;
            }
        }

        private void OnTouchMove(object sender, TouchEventArgs e)
        {
            if (AreAnyTouchesCaptured)
            {
                SetPositionByControlPoint(e.GetTouchPoint(this).Position);
                e.Handled = true;
            }
        }

        private void OnMouseMove(object sender, MouseEventArgs e)
        {
            if (e.LeftButton == MouseButtonState.Pressed && _clickedInSlider)
            {
                _thumb.RaiseEvent(new MouseButtonEventArgs(e.MouseDevice, e.Timestamp, MouseButton.Left)
                {
                    RoutedEvent = MouseLeftButtonDownEvent,
                    Source = e.Source
                });
            }
        }

        public void SetPositionByControlPoint(Point point)
        {
            var percent = point.X / ActualWidth;
            Value = Bound((Maximum - Minimum) * percent);
        }

        private void OnMouseWheel(object sender, MouseWheelEventArgs e)
        {
            var amount = Math.Sign(e.Delta) * 2.0;
            ChangePositionByAmount(amount);
            e.Handled = true;
        }

        public void ChangePositionByAmount(double amount)
        {
            Value = Bound(Value + amount);
        }

        public double Bound(double val)
        {
            return Math.Max(Minimum, Math.Min(Maximum, val));
        }
    }
}
