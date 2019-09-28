#include "pch.h"
#include "MatrixExt.h"

#define _USE_MATH_DEFINES
#include <math.h>

using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Controls;
using namespace winrt::Windows::UI::Xaml::Media;
using namespace winrt::Windows::UI::Xaml::Markup;

bool MatrixExt::HasInverse(const Matrix& matrix)
{
	return ((matrix.M11 * matrix.M22) - (matrix.M12 * matrix.M21)) != 0;
}

Matrix MatrixExt::Multiply(const Matrix& matrix1, const Matrix& matrix2)
{
	return Matrix{
		(matrix1.M11 * matrix2.M11) + (matrix1.M12 * matrix2.M21),
		(matrix1.M11 * matrix2.M12) + (matrix1.M12 * matrix2.M22),
		(matrix1.M21 * matrix2.M11) + (matrix1.M22 * matrix2.M21),
		(matrix1.M21 * matrix2.M12) + (matrix1.M22 * matrix2.M22),
		((matrix1.OffsetX * matrix2.M11) + (matrix1.OffsetY * matrix2.M21)) + matrix2.OffsetX,
		((matrix1.OffsetX * matrix2.M12) + (matrix1.OffsetY * matrix2.M22)) + matrix2.OffsetY };
}

Matrix MatrixExt::Rotate(const Matrix& matrix, const double& angle)
{
	return Multiply(matrix, CreateRotationRadians((std::fmod(angle, 360)) * (M_PI / 180.0)));
}

Matrix MatrixExt::RotateAt(const Matrix& matrix, const double& angle, const double& centerX, const double& centerY)
{
	return Multiply(matrix, CreateRotationRadians((std::fmod(angle, 360)) * (M_PI / 180.0), centerX, centerY));
}

Matrix MatrixExt::Scale(const Matrix& matrix, const double& scaleX, const double& scaleY)
{
	return Multiply(matrix, CreateScaling(scaleX, scaleY));
}

Matrix MatrixExt::ScaleAt(const Matrix& matrix, const double& scaleX, const double& scaleY, const double& centerX, const double& centerY)
{
	return Multiply(matrix, CreateScaling(scaleX, scaleY, centerX, centerY));
}

Matrix MatrixExt::Skew(const Matrix& matrix, const double& skewX, const double& skewY)
{
	return Multiply(matrix, CreateSkewRadians(std::fmod(skewX, 360) * (M_PI / 180.0), std::fmod(skewY, 360) * (M_PI / 180.0)));
}

Matrix MatrixExt::Translate(const Matrix& matrix, const double& offsetX, const double& offsetY)
{
	return Matrix{matrix.M11, matrix.M12, matrix.M21, matrix.M22, matrix.OffsetX + offsetX, matrix.OffsetY + offsetY };
}

Matrix MatrixExt::Round(const Matrix& matrix, const int& decimalsAfterRound)
{
	const auto pow_num = std::pow(10, decimalsAfterRound);

	return Matrix{
		std::round(matrix.M11 * pow_num) / pow_num,
		std::round(matrix.M12 * pow_num) / pow_num,
		std::round(matrix.M21 * pow_num) / pow_num,
		std::round(matrix.M22 * pow_num) / pow_num,
		matrix.OffsetX,
		matrix.OffsetY };
}

Matrix MatrixExt::CreateRotationRadians(const double& angle)
{
	return CreateRotationRadians(angle, 0, 0);
}

Matrix MatrixExt::CreateRotationRadians(const double& angle, const double& centerX, const double& centerY)
{
	const auto sin = std::sin(angle);
	const auto cos = std::cos(angle);
	const auto dx = (centerX * (1.0 - cos)) + (centerY * sin);
	const auto dy = (centerY * (1.0 - cos)) - (centerX * sin);
	
	return Matrix{cos, sin, -sin, cos, dx, dy};
}

Matrix MatrixExt::CreateScaling(const double& scaleX, const double& scaleY)
{
	return Matrix{ scaleX, 0, 0, scaleY, 0, 0 };
}

Matrix MatrixExt::CreateScaling(const double& scaleX, const double& scaleY, const double& centerX, const double& centerY)
{
	return Matrix{ scaleX, 0, 0, scaleY, centerX - (scaleX * centerX), centerY - (scaleY * centerY) };
}

Matrix MatrixExt::CreateSkewRadians(const double& skewX, const double& skewY)
{
	return Matrix{ 1.0, std::tan(skewY), std::tan(skewX), 1.0, 0.0, 0.0 };
}
