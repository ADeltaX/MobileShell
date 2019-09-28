#pragma once

class LayoutTransformControl : public winrt::Windows::UI::Xaml::Controls::ContentControl
{
public:
	winrt::Windows::UI::Xaml::Media::Transform Transform();
	void Transform(winrt::Windows::UI::Xaml::Media::Transform const& transform);
	void MatrixTransform(winrt::Windows::UI::Xaml::Media::MatrixTransform const& matrixTransform);
	winrt::Windows::UI::Xaml::Media::MatrixTransform MatrixTransform();
	LayoutTransformControl();
	void TransformUpdated();
	static bool IsSizeSmaller(winrt::Windows::Foundation::Size a, winrt::Windows::Foundation::Size b);
	void ProcessTransform();
	winrt::Windows::UI::Xaml::Media::Matrix GetTransformMatrix(winrt::Windows::UI::Xaml::Media::Transform transform);
	static winrt::Windows::UI::Xaml::Media::Matrix GetMatrix(winrt::Windows::UI::Xaml::Media::RotateTransform transform);
	static winrt::Windows::UI::Xaml::Media::Matrix GetMatrix(const winrt::Windows::UI::Xaml::Media::ScaleTransform transform);
	static winrt::Windows::UI::Xaml::Media::Matrix GetMatrix(const winrt::Windows::UI::Xaml::Media::SkewTransform transform);
	static winrt::Windows::UI::Xaml::Media::Matrix GetMatrix(const winrt::Windows::UI::Xaml::Media::TranslateTransform transform);
	static winrt::Windows::Foundation::Rect RectTransform(winrt::Windows::Foundation::Rect rectangle, winrt::Windows::UI::Xaml::Media::Matrix matrix);
	static winrt::Windows::Foundation::Size ComputeLargestTransformedSize(winrt::Windows::Foundation::Size arrangeBounds);
protected:
	winrt::Windows::Foundation::Size MeasureOverride(winrt::Windows::Foundation::Size const& availableSize) const;
	winrt::Windows::Foundation::Size ArrangeOverride(winrt::Windows::Foundation::Size const& finalSize) const;
	void OnContentChanged(IInspectable const& oldContent, IInspectable const& newContent) const;
	
};

