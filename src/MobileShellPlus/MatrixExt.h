#pragma once

using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Controls;
using namespace winrt::Windows::UI::Xaml::Media;
using namespace winrt::Windows::UI::Xaml::Markup;

class MatrixExt
{
public:
	static bool HasInverse(const Matrix& matrix);
	static Matrix Multiply(const Matrix& matrix1, const Matrix& matrix2);
	static Matrix Rotate(const Matrix& matrix, const double& angle);
	static Matrix RotateAt(const Matrix& matrix, const double& angle, const double& centerX, const double& centerY);
	static Matrix Scale(const Matrix& matrix, const double& scaleX, const double& scaleY);
	static Matrix ScaleAt(const Matrix& matrix, const double& scaleX, const double& scaleY, const double& centerX, const double& centerY);
	static Matrix Skew(const Matrix& matrix, const double& skewX, const double& skewY);
	static Matrix Translate(const Matrix& matrix, const double& offsetX, const double& offsetY);
	static Matrix Round(const Matrix& matrix, const int& decimalsAfterRound);

private:
	static Matrix CreateRotationRadians(const double& angle);
	static Matrix CreateRotationRadians(const double& angle, const double& centerX, const double& centerY);
	static Matrix CreateScaling(const double& scaleX, const double& scaleY);
	static Matrix CreateScaling(const double& scaleX, const double& scaleY, const double& centerX, const double& centerY);
	static Matrix CreateSkewRadians(const double& skewX, const double& skewY);
};
