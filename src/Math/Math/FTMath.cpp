// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
//
// Released under the BSD License
// https://github.com/gameprogcpp/code
// ----------------------------------------------------------------
// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
#include "FTMath.h"

namespace Math
{
	const FTVector2 FTVector2::Zero(0.0f, 0.0f);
	const FTVector2 FTVector2::UnitX(1.0f, 0.0f);
	const FTVector2 FTVector2::UnitY(0.0f, 1.0f);
	const FTVector2 FTVector2::NegUnitX(-1.0f, 0.0f);
	const FTVector2 FTVector2::NegUnitY(0.0f, -1.0f);

	const FTVector3 FTVector3::Zero(0.0f, 0.0f, 0.f);
	const FTVector3 FTVector3::UnitX(1.0f, 0.0f, 0.0f);
	const FTVector3 FTVector3::UnitY(0.0f, 1.0f, 0.0f);
	const FTVector3 FTVector3::UnitZ(0.0f, 0.0f, 1.0f);
	const FTVector3 FTVector3::NegUnitX(-1.0f, 0.0f, 0.0f);
	const FTVector3 FTVector3::NegUnitY(0.0f, -1.0f, 0.0f);
	const FTVector3 FTVector3::NegUnitZ(0.0f, 0.0f, -1.0f);
	const FTVector3 FTVector3::Infinity(Math::Infinity, Math::Infinity, Math::Infinity);
	const FTVector3 FTVector3::NegInfinity(Math::NegInfinity, Math::NegInfinity, Math::NegInfinity);

	static float m3Ident[3][3] = {
		{ 1.0f, 0.0f, 0.0f },
		{ 0.0f, 1.0f, 0.0f },
		{ 0.0f, 0.0f, 1.0f }
	};
	const FTMatrix3 FTMatrix3::Identity(m3Ident);

	static float m4Ident[4][4] = {
		{ 1.0f, 0.0f, 0.0f, 0.0f },
		{ 0.0f, 1.0f, 0.0f, 0.0f },
		{ 0.0f, 0.0f, 1.0f, 0.0f },
		{ 0.0f, 0.0f, 0.0f, 1.0f }
	};

	const FTMatrix4 FTMatrix4::Identity(m4Ident);

	const Quaternion Quaternion::Identity(0.0f, 0.0f, 0.0f, 1.0f);

	FTVector3::FTVector3()
		: x(0.0f)
		, y(0.0f)
		, z(0.0f)
	{
	}

	FTVector3::FTVector3(float in)
		: x(in)
		, y(in)
		, z(in)
	{
	}

	FTVector3::FTVector3(float inX, float inY, float inZ)
		: x(inX)
		, y(inY)
		, z(inZ)
	{
	}

	void FTVector3::Assign(float arr[3]) const
	{
		arr[0] = this->x;
		arr[1] = this->y;
		arr[2] = this->z;
	}

	float FTVector3::LengthSq()
	{
		return (x * x + y * y + z * z);
	}

	float FTVector3::Length()
	{
		return (Math::Sqrt(LengthSq()));
	}

	void FTVector3::Normalize()
	{
		float length = Length();
		x /= length;
		y /= length;
		z /= length;
	}

	FTVector3 FTVector3::Normalize(const FTVector3& vec)
	{
		FTVector3 temp = vec;
		temp.Normalize();
		return temp;
	}

	float FTVector3::Dot(const FTVector3& a, const FTVector3& b)
	{
		return (a.x * b.x + a.y * b.y + a.z * b.z);
	}

	FTVector3 FTVector3::Cross(const FTVector3& a, const FTVector3& b)
	{
		FTVector3 temp;
		temp.x = a.y * b.z - a.z * b.y;
		temp.y = a.z * b.x - a.x * b.z;
		temp.z = a.x * b.y - a.y * b.x;
		return temp;
	}

	FTVector3 FTVector3::Lerp(const FTVector3& a, const FTVector3& b, float f)
	{
		return FTVector3(a + f * (b - a));
	}

	FTVector3 FTVector3::Reflect(const FTVector3& v, const FTVector3& n)
	{
		return v - 2.0f * FTVector3::Dot(v, n) * n;
	}

	FTVector3 FTVector3::Transform(const FTVector3& vec, const FTMatrix4& mat, float w)
	{
		FTVector3 retVal;
		retVal.x = vec.x * mat.mat[0][0] + vec.y * mat.mat[1][0] +
			vec.z * mat.mat[2][0] + w * mat.mat[3][0];
		retVal.y = vec.x * mat.mat[0][1] + vec.y * mat.mat[1][1] +
			vec.z * mat.mat[2][1] + w * mat.mat[3][1];
		retVal.z = vec.x * mat.mat[0][2] + vec.y * mat.mat[1][2] +
			vec.z * mat.mat[2][2] + w * mat.mat[3][2];
		// ignore w since we aren't returning a new value for it...
		return retVal;
	}

	FTVector2 FTVector2::PlotCircularFall(float angle, float dist)
	{
		return FTVector2(dist * Math::Cos(angle), dist * Math::Sin(angle));
	}

	FTVector2 FTVector2::Transform(const FTVector2& vec, const FTMatrix3& mat, float w)
	{
		FTVector2 retVal(0.f);
		retVal.x = vec.x * mat.mat[0][0] + vec.y * mat.mat[1][0] + w * mat.mat[2][0];
		retVal.y = vec.x * mat.mat[0][1] + vec.y * mat.mat[1][1] + w * mat.mat[2][1];
		// ignore w since we aren't returning a new value for it...
		return retVal;
	}

	void FTMatrix4::Invert()
	{
		float tmp[12];
		float src[16];
		float dst[16];
		float det;

		// Transpose matrix
		// row 1 to col 1
		src[0]	= mat[0][0];
		src[4]	= mat[0][1];
		src[8]	= mat[0][2];
		src[12] = mat[0][3];

		// row 2 to col 2
		src[1]	= mat[1][0];
		src[5]	= mat[1][1];
		src[9]	= mat[1][2];
		src[13] = mat[1][3];

		// row 3 to col 3
		src[2]	= mat[2][0];
		src[6]	= mat[2][1];
		src[10] = mat[2][2];
		src[14] = mat[2][3];

		// row 4 to col 4
		src[3]	= mat[3][0];
		src[7]	= mat[3][1];
		src[11] = mat[3][2];
		src[15] = mat[3][3];

		// Calculate cofactors
		tmp[0]	= src[10] * src[15];
		tmp[1]	= src[11] * src[14];
		tmp[2]	= src[9] * src[15];
		tmp[3]	= src[11] * src[13];
		tmp[4]	= src[9] * src[14];
		tmp[5]	= src[10] * src[13];
		tmp[6]	= src[8] * src[15];
		tmp[7]	= src[11] * src[12];
		tmp[8]	= src[8] * src[14];
		tmp[9]	= src[10] * src[12];
		tmp[10] = src[8] * src[13];
		tmp[11] = src[9] * src[12];

		dst[0] = tmp[0] * src[5] + tmp[3] * src[6] + tmp[4] * src[7];
		dst[0] -= tmp[1] * src[5] + tmp[2] * src[6] + tmp[5] * src[7];
		dst[1] = tmp[1] * src[4] + tmp[6] * src[6] + tmp[9] * src[7];
		dst[1] -= tmp[0] * src[4] + tmp[7] * src[6] + tmp[8] * src[7];
		dst[2] = tmp[2] * src[4] + tmp[7] * src[5] + tmp[10] * src[7];
		dst[2] -= tmp[3] * src[4] + tmp[6] * src[5] + tmp[11] * src[7];
		dst[3] = tmp[5] * src[4] + tmp[8] * src[5] + tmp[11] * src[6];
		dst[3] -= tmp[4] * src[4] + tmp[9] * src[5] + tmp[10] * src[6];
		dst[4] = tmp[1] * src[1] + tmp[2] * src[2] + tmp[5] * src[3];
		dst[4] -= tmp[0] * src[1] + tmp[3] * src[2] + tmp[4] * src[3];
		dst[5] = tmp[0] * src[0] + tmp[7] * src[2] + tmp[8] * src[3];
		dst[5] -= tmp[1] * src[0] + tmp[6] * src[2] + tmp[9] * src[3];
		dst[6] = tmp[3] * src[0] + tmp[6] * src[1] + tmp[11] * src[3];
		dst[6] -= tmp[2] * src[0] + tmp[7] * src[1] + tmp[10] * src[3];
		dst[7] = tmp[4] * src[0] + tmp[9] * src[1] + tmp[10] * src[2];
		dst[7] -= tmp[5] * src[0] + tmp[8] * src[1] + tmp[11] * src[2];

		tmp[0]	= src[2] * src[7];
		tmp[1]	= src[3] * src[6];
		tmp[2]	= src[1] * src[7];
		tmp[3]	= src[3] * src[5];
		tmp[4]	= src[1] * src[6];
		tmp[5]	= src[2] * src[5];
		tmp[6]	= src[0] * src[7];
		tmp[7]	= src[3] * src[4];
		tmp[8]	= src[0] * src[6];
		tmp[9]	= src[2] * src[4];
		tmp[10] = src[0] * src[5];
		tmp[11] = src[1] * src[4];

		dst[8] = tmp[0] * src[13] + tmp[3] * src[14] + tmp[4] * src[15];
		dst[8] -= tmp[1] * src[13] + tmp[2] * src[14] + tmp[5] * src[15];
		dst[9] = tmp[1] * src[12] + tmp[6] * src[14] + tmp[9] * src[15];
		dst[9] -= tmp[0] * src[12] + tmp[7] * src[14] + tmp[8] * src[15];
		dst[10] = tmp[2] * src[12] + tmp[7] * src[13] + tmp[10] * src[15];
		dst[10] -= tmp[3] * src[12] + tmp[6] * src[13] + tmp[11] * src[15];
		dst[11] = tmp[5] * src[12] + tmp[8] * src[13] + tmp[11] * src[14];
		dst[11] -= tmp[4] * src[12] + tmp[9] * src[13] + tmp[10] * src[14];
		dst[12] = tmp[2] * src[10] + tmp[5] * src[11] + tmp[1] * src[9];
		dst[12] -= tmp[4] * src[11] + tmp[0] * src[9] + tmp[3] * src[10];
		dst[13] = tmp[8] * src[11] + tmp[0] * src[8] + tmp[7] * src[10];
		dst[13] -= tmp[6] * src[10] + tmp[9] * src[11] + tmp[1] * src[8];
		dst[14] = tmp[6] * src[9] + tmp[11] * src[11] + tmp[3] * src[8];
		dst[14] -= tmp[10] * src[11] + tmp[2] * src[8] + tmp[7] * src[9];
		dst[15] = tmp[10] * src[10] + tmp[4] * src[8] + tmp[9] * src[9];
		dst[15] -= tmp[8] * src[9] + tmp[11] * src[10] + tmp[5] * src[8];

		// Calculate determinant
		det = src[0] * dst[0] + src[1] * dst[1] + src[2] * dst[2] + src[3] * dst[3];

		// Inverse of matrix is divided by determinant
		det = 1 / det;
		for (int j = 0; j < 16; j++)
		{
			dst[j] *= det;
		}

		// Set it back
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				mat[i][j] = dst[i * 4 + j];
			}
		}
	}

	void FTMatrix4::Transpose()
	{
		__m128 row0 = _mm_load_ps(mat[0]);
		__m128 row1 = _mm_load_ps(mat[1]);
		__m128 row2 = _mm_load_ps(mat[2]);
		__m128 row3 = _mm_load_ps(mat[3]);

		_MM_TRANSPOSE4_PS(row0, row1, row2, row3); // modifies in-place

		_mm_store_ps(mat[0], row0);
		_mm_store_ps(mat[1], row1);
		_mm_store_ps(mat[2], row2);
		_mm_store_ps(mat[3], row3);
	}

	FTMatrix4 FTMatrix4::Transposed() const
	{
		FTMatrix4 result = *this;
		result.Transpose();
		return result;
	}
} // namespace Math