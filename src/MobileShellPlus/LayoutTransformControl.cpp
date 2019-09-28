#include "pch.h"
#include "LayoutTransformControl.h"
#include "winrt/Windows.UI.Xaml.Media.h"
#include "MatrixExt.h"
#include <cstdint>
#include <limits.h>

using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Controls;
using namespace winrt::Windows::UI::Xaml::Media;
using namespace winrt::Windows::UI::Xaml::Markup;


const double AcceptableDelta = 0.0001;
const int DecimalsAfterRound = 4;

//UWU 

Panel _layoutRoot = nullptr;
MatrixTransform _matrixTransform = nullptr;
Matrix _transformation;
Size _childActualSize = Size(0,0);
Transform _transform = nullptr;

Transform LayoutTransformControl::Transform()
{
	return _transform;
}

void LayoutTransformControl::Transform(winrt::Windows::UI::Xaml::Media::Transform const& transform)
{	
	_transform = transform;

	ProcessTransform();
}

void LayoutTransformControl::OnContentChanged(IInspectable const& oldContent, IInspectable const& newContent) const
{
	if (_layoutRoot != nullptr)
	{
		_layoutRoot.Children().Clear();
		if (newContent != nullptr)
		{
			_layoutRoot.Children().Append(newContent.as<FrameworkElement>());
		}

		InvalidateMeasure();
	}
}

MatrixTransform LayoutTransformControl::MatrixTransform()
{
	return _matrixTransform;
}

void LayoutTransformControl::MatrixTransform(winrt::Windows::UI::Xaml::Media::MatrixTransform const& matrixTransform)
{
	_matrixTransform = matrixTransform;

	TransformUpdated();
}

LayoutTransformControl::LayoutTransformControl() : ContentControl()
{
	IsTabStop(false);

	UseLayoutRounding(false);
}

void LayoutTransformControl::TransformUpdated()
{
	ProcessTransform();
}

bool LayoutTransformControl::IsSizeSmaller(Size a, Size b)
{
	return (a.Width + AcceptableDelta < b.Width) || (a.Height + AcceptableDelta < b.Height);
}

void LayoutTransformControl::ProcessTransform()
{
	_transformation = MatrixExt::Round(GetTransformMatrix(_transform), DecimalsAfterRound);

	if (_matrixTransform != nullptr)
	{
		_matrixTransform.Matrix(_transformation);
	}

	this->InvalidateMeasure();
}

Matrix LayoutTransformControl::GetTransformMatrix(winrt::Windows::UI::Xaml::Media::Transform transform)
{
	if (transform != nullptr)
	{
		//auto transformGroup = transform.as<TransformGroup>();

		//if (transformGroup != nullptr)
		//{
		//	auto groupMatrix = MatrixHelper::Identity();
		//	
		//	for (auto const& child : transformGroup.Children())
		//	{
		//		groupMatrix = MatrixExt::Multiply(groupMatrix, GetTransformMatrix(child));
		//	}

		//	return groupMatrix;
		//}

		// Process the RotateTransform
		const auto rotateTransform = transform.as<RotateTransform>();

		if (rotateTransform != nullptr)
		{		
			return GetMatrix(rotateTransform);
		}

		// Process the ScaleTransform
		const auto scaleTransform = transform.as<ScaleTransform>();

		if (scaleTransform != nullptr)
		{
			return GetMatrix(scaleTransform);
		}

		// Process the SkewTransform
		const auto  skewTransform = transform.as<SkewTransform>();;

		if (skewTransform != nullptr)
		{
			return GetMatrix(skewTransform);
		}

		// Process the MatrixTransform
		const auto  matrixTransform = transform.as<winrt::Windows::UI::Xaml::Media::MatrixTransform>();
		if (matrixTransform != nullptr)
		{
			return matrixTransform.Matrix();
		}
	}

	return MatrixHelper::Identity();
}

Size LayoutTransformControl::MeasureOverride(Size const& availableSize) const
{
	FrameworkElement child = Content().as<FrameworkElement>();
	if (_layoutRoot == nullptr || child == nullptr)
	{
		return Size(0, 0);
	}

	Size measureSize;
	if (_childActualSize == Size(0, 0))
	{
		// Determine the largest size after the transformation
		measureSize = ComputeLargestTransformedSize(availableSize);
	}
	else
	{
		// Previous measure/arrange pass determined that Child.DesiredSize was larger than believed.
		measureSize = _childActualSize;
	}

	// Perform a mesaure on the _layoutRoot (containing Child)
	_layoutRoot.Measure(measureSize);

	// Transform DesiredSize to find its width/height
	const auto transformedDesiredRect = RectTransform(Rect(0, 0, _layoutRoot.DesiredSize().Width, _layoutRoot.DesiredSize().Height), _transformation);
	const auto transformedDesiredSize = Size(transformedDesiredRect.Width, transformedDesiredRect.Height);

	// Return result to allocate enough space for the transformation
	return transformedDesiredSize;
}

Size LayoutTransformControl::ArrangeOverride(Size const& finalSize) const
{
	FrameworkElement child = Content().as<FrameworkElement>();
	if (_layoutRoot == nullptr || child == nullptr)
	{
		// No child, use whatever was given
		return finalSize;
	}

	// Determine the largest available size after the transformation
	Size finalSizeTransformed = ComputeLargestTransformedSize(finalSize);
	if (IsSizeSmaller(finalSizeTransformed, _layoutRoot.DesiredSize()))
	{
		// Some elements do not like being given less space than they asked for (ex: TextBlock)
		// Bump the working size up to do the right thing by them
		finalSizeTransformed = _layoutRoot.DesiredSize();
	}

	// Transform the working size to find its width/height
	Rect transformedRect = RectTransform(Rect(0, 0, finalSizeTransformed.Width, finalSizeTransformed.Height), _transformation);

	// Create the Arrange rect to center the transformed content
	Rect finalRect = Rect(
		-transformedRect.X + ((finalSize.Width - transformedRect.Width) / 2),
		-transformedRect.Y + ((finalSize.Height - transformedRect.Height) / 2),
		finalSizeTransformed.Width,
		finalSizeTransformed.Height);

	// Perform an Arrange on _layoutRoot (containing Child)
	_layoutRoot.Arrange(finalRect);

	// This is the first opportunity to find out the Child's true DesiredSize
	if (IsSizeSmaller(finalSizeTransformed, child.RenderSize()) && (Size(0, 0) == _childActualSize))
	{
		// Unfortunately, all the work so far is invalid because the wrong DesiredSize was used
		// Make a note of the actual DesiredSize
		_childActualSize = Size(child.ActualWidth(), child.ActualHeight());

		// Force a new measure/arrange pass
		InvalidateMeasure();
	}
	else
	{
		// Clear the "need to measure/arrange again" flag
		_childActualSize = Size(0, 0);
	}

	// Return result to perform the transformation
	return finalSize;
}

Size LayoutTransformControl::ComputeLargestTransformedSize(Size arrangeBounds)
{
	// Computed largest transformed size
	auto computedSize = Size(0, 0);

	// Detect infinite bounds and constrain the scenario
	const auto infiniteWidth = std::isinf(arrangeBounds.Width);

	if (infiniteWidth)
	{
		arrangeBounds.Width = arrangeBounds.Height;
	}

	const auto infiniteHeight = std::isinf(arrangeBounds.Height);

	if (infiniteHeight)
	{
		arrangeBounds.Height = arrangeBounds.Width;
	}

	// Capture the matrix parameters
	const auto a = _transformation.M11;
	const auto b = _transformation.M12;
	const auto c = _transformation.M21;
	const auto d = _transformation.M22;

	// Compute maximum possible transformed width/height based on starting width/height
	// These constraints define two lines in the positive x/y quadrant
	const auto maxWidthFromWidth = std::abs(arrangeBounds.Width / a);
	const auto maxHeightFromWidth = std::abs(arrangeBounds.Width / c);
	const auto maxWidthFromHeight = std::abs(arrangeBounds.Height / b);
	const auto maxHeightFromHeight = std::abs(arrangeBounds.Height / d);

	// The transformed width/height that maximize the area under each segment is its midpoint
	// At most one of the two midpoints will satisfy both constraints
	const auto idealWidthFromWidth = maxWidthFromWidth / 2;
	const auto idealHeightFromWidth = maxHeightFromWidth / 2;
	const auto idealWidthFromHeight = maxWidthFromHeight / 2;
	const auto idealHeightFromHeight = maxHeightFromHeight / 2;

	// Compute slope of both constraint lines
	const auto slopeFromWidth = -(maxHeightFromWidth / maxWidthFromWidth);
	const auto slopeFromHeight = -(maxHeightFromHeight / maxWidthFromHeight);

	if (arrangeBounds.Width == 0 || arrangeBounds.Height == 0)
	{
		// Check for empty bounds
		computedSize = Size(0, 0);
	}
	else if (infiniteWidth && infiniteHeight)
	{
		// Check for completely unbound scenario
		computedSize = Size(std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity());
	}
	else if (!MatrixExt::HasInverse(_transformation))
	{
		// Check for singular matrix
		computedSize = Size(0, 0);
	}
	else if (b == 0 || c == 0)
	{
		// Check for 0/180 degree special cases
		const auto maxHeight = infiniteHeight ? std::numeric_limits<double>::infinity() : maxHeightFromHeight;
		const auto maxWidth = infiniteWidth ? std::numeric_limits<double>::infinity() : maxWidthFromWidth;

		if (b == 0 && c == 0)
		{
			// No constraints
			computedSize = Size(maxWidth, maxHeight);
		}
		else if (b == 0)
		{
			// Constrained by width
			const auto computedHeight = std::min<double>(idealHeightFromWidth, maxHeight);
			computedSize = Size(
				maxWidth - std::abs((c * computedHeight) / a),
				computedHeight);
		}
		else if (c == 0)
		{
			// Constrained by height
			const auto computedWidth = std::min<double>(idealWidthFromHeight, maxWidth);
			computedSize = Size(
				computedWidth,
				maxHeight - std::abs((b * computedWidth) / d));
		}
	}
	else if (a == 0 || d == 0)
	{
		// Check for 90/270 degree special cases
		const auto maxWidth = infiniteHeight ? std::numeric_limits<double>::infinity() : maxWidthFromHeight;
		const auto maxHeight = infiniteWidth ? std::numeric_limits<double>::infinity() : maxHeightFromWidth;

		if (a == 0 && d == 0)
		{
			// No constraints
			computedSize = Size(maxWidth, maxHeight);
		}
		else if (a == 0)
		{
			// Constrained by width
			const auto computedHeight = std::min<double>(idealHeightFromHeight, maxHeight);
			computedSize = Size(
				maxWidth - std::abs((d * computedHeight) / b),
				computedHeight);
		}
		else if (d == 0)
		{
			// Constrained by height.
			const auto computedWidth = std::min<double>(idealWidthFromWidth, maxWidth);
			computedSize = Size(
				computedWidth,
				maxHeight - std::abs((a * computedWidth) / c));
		}
	}
	else if (idealHeightFromWidth <= ((slopeFromHeight * idealWidthFromWidth) + maxHeightFromHeight))
	{
		// Check the width midpoint for viability (by being below the height constraint line).
		computedSize = Size(idealWidthFromWidth, idealHeightFromWidth);
	}
	else if (idealHeightFromHeight <= ((slopeFromWidth * idealWidthFromHeight) + maxHeightFromWidth))
	{
		// Check the height midpoint for viability (by being below the width constraint line).
		computedSize = Size(idealWidthFromHeight, idealHeightFromHeight);
	}
	else
	{
		// Neither midpoint is viable; use the intersection of the two constraint lines instead.

		// Compute width by setting heights equal (m1*x+c1=m2*x+c2).
		const auto computedWidth = (maxHeightFromHeight - maxHeightFromWidth) / (slopeFromWidth - slopeFromHeight);

		// Compute height from width constraint line (y=m*x+c; using height would give same result).
		computedSize = Size(
			computedWidth,
			(slopeFromWidth * computedWidth) + maxHeightFromWidth);
	}

	return computedSize;
}


Matrix LayoutTransformControl::GetMatrix(const RotateTransform transform)
{
	const auto matrixIdentity = MatrixHelper::Identity();
	return MatrixExt::RotateAt(matrixIdentity, transform.Angle(), transform.CenterX(), transform.CenterY());
}

Matrix LayoutTransformControl::GetMatrix(const ScaleTransform transform)
{
	const auto matrixIdentity = MatrixHelper::Identity();
	return MatrixExt::ScaleAt(matrixIdentity, transform.ScaleX(), transform.ScaleY(), transform.CenterX(), transform.CenterY());
}

Matrix LayoutTransformControl::GetMatrix(const SkewTransform transform)
{
	auto matrix = MatrixHelper::Identity();

	const auto angleX = transform.AngleX();
	const auto angleY = transform.AngleY();
	const auto centerX = transform.CenterX();
	const auto centerY = transform.CenterY();

	const bool hasCenter = centerX != 0 || centerY != 0;

	if (hasCenter)
	{
		// If we have a center, translate matrix before/after skewing.
		matrix = MatrixExt::Translate(matrix, -centerX, -centerY);
		matrix = MatrixExt::Skew(matrix, angleX, angleY);
		matrix = MatrixExt::Translate(matrix, centerX, centerY);
	}
	else
	{
		matrix = MatrixExt::Skew(matrix, angleX, angleY);
	}

	return matrix;
}

Matrix LayoutTransformControl::GetMatrix(const TranslateTransform  transform)
{
	const auto matrixIdentity = MatrixHelper::Identity();
	return MatrixExt::Translate(matrixIdentity, transform.X(), transform.Y());
}

Rect LayoutTransformControl::RectTransform(const Rect rectangle, const Matrix matrix)
{
	auto leftTop = MatrixHelper::Transform(matrix, Point(rectangle.X, rectangle.Y));
	auto rightTop = MatrixHelper::Transform(matrix, Point(rectangle.Width, rectangle.Y));
	auto leftBottom = MatrixHelper::Transform(matrix, Point(rectangle.X, rectangle.Height));
	auto rightBottom = MatrixHelper::Transform(matrix, Point(rectangle.Width, rectangle.Height));

	const auto left = std::min<double>(std::min<double>(leftTop.X, rightTop.X), std::min<double>(leftBottom.X, rightBottom.X));
	const auto top = std::min<double>(std::min<double>(leftTop.Y, rightTop.Y), std::min<double>(leftBottom.Y, rightBottom.Y));
	const auto right = std::max<double>(std::max<double>(leftTop.X, rightTop.X), std::max<double>(leftBottom.X, rightBottom.X));
	const auto bottom = std::max<double>(std::max<double>(leftTop.Y, rightTop.Y), std::max<double>(leftBottom.Y, rightBottom.Y));
	const Rect rectTransformed(left, top, right - left, bottom - top);
	return rectTransformed;
}
