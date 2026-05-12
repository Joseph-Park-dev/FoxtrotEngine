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
/// <summary>
/// Math related classes, especially Vectors and their operations.
/// </summary>

#pragma once
#include <cmath>
#include <memory.h>
#include <limits>
#include <assert.h>

namespace Math
{
	const float Pi(3.1415926535f);
	const float TwoPi(Math::Pi * 2.0f);
	const float PiOver2(Math::Pi / 2.0f);
	const float Infinity(std::numeric_limits<float>::infinity());
	const float NegInfinity(-std::numeric_limits<float>::infinity());

	inline float ToRadians(float degrees)
	{
		return degrees * Pi / 180.0f;
	}

	inline float ToDegrees(float radians)
	{
		return radians * 180.0f / Pi;
	}

	inline bool NearZero(float val, float epsilon = 0.001f)
	{
		if (fabs(val) <= epsilon)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	inline float Abs(float value)
	{
		return fabs(value);
	}

	inline size_t Min(size_t a, size_t b)
	{
		return a > b ? b : a;
	}

	inline float Cos(float angle)
	{
		return cosf(angle);
	}

	inline float Sin(float angle)
	{
		return sinf(angle);
	}

	inline float Tan(float angle)
	{
		return tanf(angle);
	}

	inline float Acos(float value)
	{
		return acosf(value);
	}

	inline float Atan2(float y, float x)
	{
		return atan2f(y, x);
	}

	inline float Cot(float angle)
	{
		return 1.0f / Tan(angle);
	}

	inline float Lerp(float a, float b, float f)
	{
		return a + f * (b - a);
	}

	inline float Sqrt(float value)
	{
		return sqrtf(value);
	}

	inline float Fmod(float numer, float denom)
	{
		return fmod(numer, denom);
	}

	inline void Clamp(int& val, int min, int max)
	{
		if (val < min)
			val = min;
		else if (max < val)
			val = max;
		else if (val == min && val == max)
			val = min;
	}

	inline void Clampf(float& val, float min, float max)
	{
		if (val < min)
			val = min;
		else if (max < val)
			val = max;
		else if (val == min && val == max)
			val = min;
	}

	inline size_t NextPowerOf2(int m)
	{
		m--;
		m |= m >> 1;
		m |= m >> 2;
		m |= m >> 4;
		m |= m >> 8;
		m |= m >> 16;
		m++;
		return m;
	}

	class FTVector2;

	class alignas(16) FTVector4
	{
	public:
		union
		{
			__m128 SIMD;
			float  Data[4];
			struct
			{
				float x, y, z, w;
			};
		};

		// Default — zero vector
		FTVector4()
			: SIMD(_mm_setzero_ps()) {}

		// 4 explicit components
		FTVector4(float x, float y, float z, float w)
			: SIMD(_mm_set_ps(w, z, y, x)) {}

		// Splat — all lanes same value
		explicit FTVector4(float s)
			: SIMD(_mm_set1_ps(s)) {}

		// Direct from __m128 (used internally)
		FTVector4(__m128 m)
			: SIMD(m) {}
	};

	class FTMatrix4;

	// 3D Vector
	class FTVector3
	{
	public:
		float x;
		float y;
		float z;

		FTVector3();
		explicit FTVector3(float in);
		explicit FTVector3(float inX, float inY, float inZ);

		// Vector addition (a + b)
		friend FTVector3 operator+(const FTVector3& a, const FTVector3& b)
		{
			return FTVector3(a.x + b.x, a.y + b.y, a.z + b.z);
		}

		// Vector subtraction (a - b)
		friend FTVector3 operator-(const FTVector3& a, const FTVector3& b)
		{
			return FTVector3(a.x - b.x, a.y - b.y, a.z - b.z);
		}

		// Component-wise multiplication
		friend FTVector3 operator*(const FTVector3& left, const FTVector3& right)
		{
			return FTVector3(left.x * right.x, left.y * right.y, left.z * right.z);
		}

		// Scalar multiplication
		friend FTVector3 operator*(const FTVector3& vec, float scalar)
		{
			return FTVector3(vec.x * scalar, vec.y * scalar, vec.z * scalar);
		}

		// Scalar multiplication
		friend FTVector3 operator*(float scalar, const FTVector3& vec)
		{
			return FTVector3(vec.x * scalar, vec.y * scalar, vec.z * scalar);
		}

		friend FTVector3 operator/(const FTVector3& vec, float scalar)
		{
			assert(scalar != 0);
			return FTVector3(vec.x / scalar, vec.y / scalar, vec.z / scalar);
		}

		friend bool operator==(const FTVector3& left, const FTVector3& right)
		{
			return (left.x == right.x) && (left.y == right.y) && (left.z == right.z);
		}

		friend bool operator!=(const FTVector3& left, const FTVector3& right)
		{
			return (left.x != right.x) || (left.y != right.y) || (left.z != right.z);
		}

		// Scalar *=
		friend FTVector3 operator*=(FTVector3& vec3, float scalar)
		{
			vec3.x *= scalar;
			vec3.y *= scalar;
			vec3.z *= scalar;
			return vec3;
		}

		// Vector +=
		friend FTVector3 operator+=(FTVector3& left, const FTVector3& right)
		{
			left.x += right.x;
			left.y += right.y;
			left.z += right.z;
			return left;
		}

		// Vector -=
		friend FTVector3 operator-=(FTVector3& left, const FTVector3& right)
		{
			left.x -= right.x;
			left.y -= right.y;
			left.z -= right.z;
			return left;
		}

		// Vector *=
		friend FTVector3 operator*=(FTVector3& left, const FTVector3& right)
		{
			left.x *= right.x;
			left.y *= right.y;
			left.z *= right.z;
			return left;
		}

		// Vector /=
		friend FTVector3 operator/=(FTVector3& left, const float& right)
		{
			left.x /= right;
			left.y /= right;
			left.z /= right;
			return left;
		}

		void Assign(float arr[3]) const;

		// Length squared of vector
		float LengthSq();

		// Length of vector
		float Length();

		// Normalize this vector
		void Normalize();

		// Normalize the provided vector
		static FTVector3 Normalize(const FTVector3& vec);

		// Dot product between two vectors (a dot b)
		static float Dot(const FTVector3& a, const FTVector3& b);

		// Cross product between two vectors (a cross b)
		static FTVector3 Cross(const FTVector3& a, const FTVector3& b);

		// Lerp from A to B by f
		static FTVector3 Lerp(const FTVector3& a, const FTVector3& b, float f);

		// Reflect V about (normalized) N
		static FTVector3 Reflect(const FTVector3& v, const FTVector3& n);

		static FTVector3 Transform(const FTVector3& vec, const FTMatrix4& mat, float w /*= 1.0f*/);

		static const FTVector3 Zero;
		static const FTVector3 UnitX;
		static const FTVector3 UnitY;
		static const FTVector3 UnitZ;
		static const FTVector3 NegUnitX;
		static const FTVector3 NegUnitY;
		static const FTVector3 NegUnitZ;
		static const FTVector3 Infinity;
		static const FTVector3 NegInfinity;
	};

	class FTVector2
	{
	public:
		union
		{
			float Data[2];
			struct
			{
				float x, y;
			};
		};

		FTVector2()
			: x(0.0f), y(0.0f) {}

		// 2 components — z,w lanes set to 0
		FTVector2(float xIn, float yIn)
			: x(xIn), y(yIn) {}

		// 2 components — z,w lanes set to 0
		FTVector2(unsigned int xIn, unsigned int yIn)
			: x(static_cast<float>(xIn)), y(static_cast<float>(yIn)) {}

		// Splat
		explicit FTVector2(float s)
			: x(s), y(s) {}

		// Vector addition (a + b)
		friend FTVector2 operator+(const FTVector2& a, const FTVector2& b)
		{
			return FTVector2(a.x + b.x, a.y + b.y);
		}

		// Vector subtraction (a - b)
		friend FTVector2 operator-(const FTVector2& a, const FTVector2& b)
		{
			return FTVector2(a.x - b.x, a.y - b.y);
		}

		// Component-wise multiplication
		// (a.x * b.x, ...)
		friend FTVector2 operator*(const FTVector2& a, const FTVector2& b)
		{
			return FTVector2(a.x * b.x, a.y * b.y);
		}

		// Scalar multiplication
		friend FTVector2 operator*(const FTVector2& vec, float scalar)
		{
			return FTVector2(vec.x * scalar, vec.y * scalar);
		}

		// Scalar multiplication
		friend FTVector2 operator*(float scalar, const FTVector2& vec)
		{
			return FTVector2(vec.x * scalar, vec.y * scalar);
		}

		FTVector2 operator/(FTVector2 vOther)
		{
			assert(!(vOther.x == 0.f || vOther.y == 0.f));
			return FTVector2(x / vOther.x, y / vOther.y);
		}

		FTVector2 operator/(float scalar)
		{
			assert(!(scalar == 0.f));
			return FTVector2(x / scalar, y / scalar);
		}

		void operator/=(float scalar)
		{
			assert(!(scalar == 0.f));
			this->x /= scalar;
			this->y /= scalar;
		}

		// Scalar *=
		FTVector2& operator*=(float scalar)
		{
			x *= scalar;
			y *= scalar;
			return *this;
		}

		FTVector2& operator*=(FTVector2 vec2)
		{
			x = x * vec2.x;
			y = y * vec2.y;
			return *this;
		}

		// Vector +=
		FTVector2& operator+=(const FTVector2& right)
		{
			x += right.x;
			y += right.y;
			return *this;
		}

		// Vector -=
		FTVector2& operator-=(const FTVector2& right)
		{
			x -= right.x;
			y -= right.y;
			return *this;
		}

		FTVector2 operator-()
		{
			return FTVector2(-x, -y);
		}

		bool operator==(const FTVector2& right)
		{
			return (this->x == right.x) && (this->y == right.y);
		}

		bool operator!=(const FTVector2& right)
		{
			return (this->x != right.x) || (this->y != right.y);
		}

		void addScaledVector(const FTVector2& vector, float scale)
		{
			x += vector.x * scale;
			y += vector.y * scale;
		}

		bool IsZero() const
		{
			return (x == 0.f && y == 0.f);
		}

		// Length squared of vector
		float LengthSq() const
		{
			return (x * x + y * y);
		}

		// Length of vector
		float Length() const
		{
			return (Math::Sqrt(LengthSq()));
		}

		// Normalize this vector
		FTVector2 Normalize()
		{
			float length = Length();
			x /= length;
			y /= length;
			return *this;
		}

		// Normalize the provided vector
		static FTVector2 Normalize(const FTVector2& vec)
		{
			FTVector2 temp = vec;
			temp.Normalize();
			return temp;
		}

		// Dot product between two vectors (a dot b)
		static float Dot(const FTVector2& a, const FTVector2& b)
		{
			return (a.x * b.x + a.y * b.y);
		}

		// Lerp from A to B by f
		static FTVector2 Lerp(const FTVector2& a, const FTVector2& b, float f)
		{
			return FTVector2(a + f * (b - a));
		}

		// Reflect V about (normalized) N
		static FTVector2 Reflect(const FTVector2& v, const FTVector2& n)
		{
			return v - 2.0f * FTVector2::Dot(v, n) * n;
		}

		static FTVector2 PlotCircularFall(float angle, float dist);

		static const FTVector2 Zero;
		static const FTVector2 UnitX;
		static const FTVector2 UnitY;
		static const FTVector2 NegUnitX;
		static const FTVector2 NegUnitY;

		// Transform vector by matrix
		static FTVector2 Transform(const FTVector2& vec, const class FTMatrix3& mat, float w = 1.0f);

		// SIMD
		static inline void Add_Vec_X2(
			const FTVector2& a0, const FTVector2& b0, const FTVector2& a1, const FTVector2& b1, FTVector2& out0, FTVector2& out1)
		{
			// Load a0 into low 64 bits, a1 into high 64 bits → [X0, Y0, X1, Y1]
			__m128 a = _mm_movelh_ps(
				_mm_loadl_pi(_mm_setzero_ps(), (__m64*)a0.Data),
				_mm_loadl_pi(_mm_setzero_ps(), (__m64*)a1.Data));

			__m128 b = _mm_movelh_ps(
				_mm_loadl_pi(_mm_setzero_ps(), (__m64*)b0.Data),
				_mm_loadl_pi(_mm_setzero_ps(), (__m64*)b1.Data));

			// One instruction — adds all 4 floats simultaneously
			// [X0+X0, Y0+Y0, X1+X1, Y1+Y1]
			__m128 res = _mm_add_ps(a, b);

			// Unpack back to two FTVector2s
			_mm_storel_pi((__m64*)out0.Data, res); // low  64 bits → out0
			_mm_storeh_pi((__m64*)out1.Data, res); // high 64 bits → out1
		}

		static inline void Sub_Vec_X2(
			const FTVector2& a0, const FTVector2& b0, const FTVector2& a1, const FTVector2& b1, FTVector2& out0, FTVector2& out1)
		{
			// Load a0 into low 64 bits, a1 into high 64 bits → [X0, Y0, X1, Y1]
			__m128 a = _mm_movelh_ps(
				_mm_loadl_pi(_mm_setzero_ps(), (__m64*)a0.Data),
				_mm_loadl_pi(_mm_setzero_ps(), (__m64*)a1.Data));

			__m128 b = _mm_movelh_ps(
				_mm_loadl_pi(_mm_setzero_ps(), (__m64*)b0.Data),
				_mm_loadl_pi(_mm_setzero_ps(), (__m64*)b1.Data));

			// One instruction — subtracts all 4 floats simultaneously
			// [X0+X0, Y0+Y0, X1+X1, Y1+Y1]
			__m128 res = _mm_sub_ps(a, b);

			// Unpack back to two FTVector2s
			_mm_storel_pi((__m64*)out0.Data, res); // low  64 bits → out0
			_mm_storeh_pi((__m64*)out1.Data, res); // high 64 bits → out1
		}

		static inline void Mul_Vec_X2(
			const FTVector2& a0, const FTVector2& b0, const FTVector2& a1, const FTVector2& b1, FTVector2& out0, FTVector2& out1)
		{
			// Load a0 into low 64 bits, a1 into high 64 bits → [X0, Y0, X1, Y1]
			__m128 a = _mm_movelh_ps(
				_mm_loadl_pi(_mm_setzero_ps(), (__m64*)a0.Data),
				_mm_loadl_pi(_mm_setzero_ps(), (__m64*)a1.Data));

			__m128 b = _mm_movelh_ps(
				_mm_loadl_pi(_mm_setzero_ps(), (__m64*)b0.Data),
				_mm_loadl_pi(_mm_setzero_ps(), (__m64*)b1.Data));

			// One instruction — multiplies all 4 floats simultaneously
			__m128 res = _mm_mul_ps(a, b);

			// Unpack back to two FTVector2s
			_mm_storel_pi((__m64*)out0.Data, res); // low  64 bits → out0
			_mm_storeh_pi((__m64*)out1.Data, res); // high 64 bits → out1
		}

		static inline void Div_Vec_X2(
			const FTVector2& a0, const FTVector2& b0, const FTVector2& a1, const FTVector2& b1, FTVector2& out0, FTVector2& out1)
		{
			// Load a0 into low 64 bits, a1 into high 64 bits → [X0, Y0, X1, Y1]
			__m128 a = _mm_movelh_ps(
				_mm_loadl_pi(_mm_setzero_ps(), (__m64*)a0.Data),
				_mm_loadl_pi(_mm_setzero_ps(), (__m64*)a1.Data));

			__m128 b = _mm_movelh_ps(
				_mm_loadl_pi(_mm_setzero_ps(), (__m64*)b0.Data),
				_mm_loadl_pi(_mm_setzero_ps(), (__m64*)b1.Data));

			// One instruction — divides all 4 floats simultaneously
			__m128 res = _mm_div_ps(a, b);

			// Unpack back to two FTVector2s
			_mm_storel_pi((__m64*)out0.Data, res); // low  64 bits → out0
			_mm_storeh_pi((__m64*)out1.Data, res); // high 64 bits → out1
		}
	};

	// 3x3 Matrix
	class FTMatrix3
	{
	public:
		float mat[3][3];

		FTMatrix3()
		{
			*this = FTMatrix3::Identity;
		}

		explicit FTMatrix3(float inMat[3][3])
		{
			memcpy(mat, inMat, 9 * sizeof(float));
		}

		// Cast to a const float pointer
		const float* GetAsFloatPtr() const
		{
			return reinterpret_cast<const float*>(&mat[0][0]);
		}

		// Matrix multiplication
		friend FTMatrix3 operator*(const FTMatrix3& left, const FTMatrix3& right)
		{
			FTMatrix3 retVal;
			// row 0
			retVal.mat[0][0] =
				left.mat[0][0] * right.mat[0][0] +
				left.mat[0][1] * right.mat[1][0] +
				left.mat[0][2] * right.mat[2][0];

			retVal.mat[0][1] =
				left.mat[0][0] * right.mat[0][1] +
				left.mat[0][1] * right.mat[1][1] +
				left.mat[0][2] * right.mat[2][1];

			retVal.mat[0][2] =
				left.mat[0][0] * right.mat[0][2] +
				left.mat[0][1] * right.mat[1][2] +
				left.mat[0][2] * right.mat[2][2];

			// row 1
			retVal.mat[1][0] =
				left.mat[1][0] * right.mat[0][0] +
				left.mat[1][1] * right.mat[1][0] +
				left.mat[1][2] * right.mat[2][0];

			retVal.mat[1][1] =
				left.mat[1][0] * right.mat[0][1] +
				left.mat[1][1] * right.mat[1][1] +
				left.mat[1][2] * right.mat[2][1];

			retVal.mat[1][2] =
				left.mat[1][0] * right.mat[0][2] +
				left.mat[1][1] * right.mat[1][2] +
				left.mat[1][2] * right.mat[2][2];

			// row 2
			retVal.mat[2][0] =
				left.mat[2][0] * right.mat[0][0] +
				left.mat[2][1] * right.mat[1][0] +
				left.mat[2][2] * right.mat[2][0];

			retVal.mat[2][1] =
				left.mat[2][0] * right.mat[0][1] +
				left.mat[2][1] * right.mat[1][1] +
				left.mat[2][2] * right.mat[2][1];

			retVal.mat[2][2] =
				left.mat[2][0] * right.mat[0][2] +
				left.mat[2][1] * right.mat[1][2] +
				left.mat[2][2] * right.mat[2][2];

			return retVal;
		}

		FTMatrix3& operator*=(const FTMatrix3& right)
		{
			*this = *this * right;
			return *this;
		}

		// Create a scale matrix with x and y scales
		static FTMatrix3 CreateScale(float xScale, float yScale)
		{
			float temp[3][3] = {
				{ xScale, 0.0f, 0.0f },
				{ 0.0f, yScale, 0.0f },
				{ 0.0f, 0.0f, 1.0f },
			};
			return FTMatrix3(temp);
		}

		static FTMatrix3 CreateScale(const FTVector2& scaleVector)
		{
			return CreateScale(scaleVector.x, scaleVector.y);
		}

		// Create a scale matrix with a uniform factor
		static FTMatrix3 CreateScale(float scale)
		{
			return CreateScale(scale, scale);
		}

		// Create a rotation matrix about the Z axis
		// theta is in radians
		static FTMatrix3 CreateRotation(float theta)
		{
			float temp[3][3] = {
				{ Math::Cos(theta), Math::Sin(theta), 0.0f },
				{ -Math::Sin(theta), Math::Cos(theta), 0.0f },
				{ 0.0f, 0.0f, 1.0f },
			};
			return FTMatrix3(temp);
		}

		// Create a translation matrix (on the xy-plane)
		static FTMatrix3 CreateTranslation(const FTVector2& trans)
		{
			float temp[3][3] = {
				{ 1.0f, 0.0f, 0.0f },
				{ 0.0f, 1.0f, 0.0f },
				{ trans.x, trans.y, 1.0f },
			};
			return FTMatrix3(temp);
		}

		static const FTMatrix3 Identity;
	};

	// 4x4 Matrix
	class FTMatrix4
	{
	public:
		float mat[4][4];

		FTMatrix4()
		{
			*this = FTMatrix4::Identity;
		}

		explicit FTMatrix4(float inMat[4][4])
		{
			memcpy(mat, inMat, 16 * sizeof(float));
		}

		// Cast to a const float pointer
		const float* GetAsFloatPtr() const
		{
			return reinterpret_cast<const float*>(&mat[0][0]);
		}

		// Matrix multiplication (a * b)
		friend FTMatrix4 operator*(const FTMatrix4& a, const FTMatrix4& b)
		{
			FTMatrix4 retVal;
			// row 0
			retVal.mat[0][0] =
				a.mat[0][0] * b.mat[0][0] +
				a.mat[0][1] * b.mat[1][0] +
				a.mat[0][2] * b.mat[2][0] +
				a.mat[0][3] * b.mat[3][0];

			retVal.mat[0][1] =
				a.mat[0][0] * b.mat[0][1] +
				a.mat[0][1] * b.mat[1][1] +
				a.mat[0][2] * b.mat[2][1] +
				a.mat[0][3] * b.mat[3][1];

			retVal.mat[0][2] =
				a.mat[0][0] * b.mat[0][2] +
				a.mat[0][1] * b.mat[1][2] +
				a.mat[0][2] * b.mat[2][2] +
				a.mat[0][3] * b.mat[3][2];

			retVal.mat[0][3] =
				a.mat[0][0] * b.mat[0][3] +
				a.mat[0][1] * b.mat[1][3] +
				a.mat[0][2] * b.mat[2][3] +
				a.mat[0][3] * b.mat[3][3];

			// row 1
			retVal.mat[1][0] =
				a.mat[1][0] * b.mat[0][0] +
				a.mat[1][1] * b.mat[1][0] +
				a.mat[1][2] * b.mat[2][0] +
				a.mat[1][3] * b.mat[3][0];

			retVal.mat[1][1] =
				a.mat[1][0] * b.mat[0][1] +
				a.mat[1][1] * b.mat[1][1] +
				a.mat[1][2] * b.mat[2][1] +
				a.mat[1][3] * b.mat[3][1];

			retVal.mat[1][2] =
				a.mat[1][0] * b.mat[0][2] +
				a.mat[1][1] * b.mat[1][2] +
				a.mat[1][2] * b.mat[2][2] +
				a.mat[1][3] * b.mat[3][2];

			retVal.mat[1][3] =
				a.mat[1][0] * b.mat[0][3] +
				a.mat[1][1] * b.mat[1][3] +
				a.mat[1][2] * b.mat[2][3] +
				a.mat[1][3] * b.mat[3][3];

			// row 2
			retVal.mat[2][0] =
				a.mat[2][0] * b.mat[0][0] +
				a.mat[2][1] * b.mat[1][0] +
				a.mat[2][2] * b.mat[2][0] +
				a.mat[2][3] * b.mat[3][0];

			retVal.mat[2][1] =
				a.mat[2][0] * b.mat[0][1] +
				a.mat[2][1] * b.mat[1][1] +
				a.mat[2][2] * b.mat[2][1] +
				a.mat[2][3] * b.mat[3][1];

			retVal.mat[2][2] =
				a.mat[2][0] * b.mat[0][2] +
				a.mat[2][1] * b.mat[1][2] +
				a.mat[2][2] * b.mat[2][2] +
				a.mat[2][3] * b.mat[3][2];

			retVal.mat[2][3] =
				a.mat[2][0] * b.mat[0][3] +
				a.mat[2][1] * b.mat[1][3] +
				a.mat[2][2] * b.mat[2][3] +
				a.mat[2][3] * b.mat[3][3];

			// row 3
			retVal.mat[3][0] =
				a.mat[3][0] * b.mat[0][0] +
				a.mat[3][1] * b.mat[1][0] +
				a.mat[3][2] * b.mat[2][0] +
				a.mat[3][3] * b.mat[3][0];

			retVal.mat[3][1] =
				a.mat[3][0] * b.mat[0][1] +
				a.mat[3][1] * b.mat[1][1] +
				a.mat[3][2] * b.mat[2][1] +
				a.mat[3][3] * b.mat[3][1];

			retVal.mat[3][2] =
				a.mat[3][0] * b.mat[0][2] +
				a.mat[3][1] * b.mat[1][2] +
				a.mat[3][2] * b.mat[2][2] +
				a.mat[3][3] * b.mat[3][2];

			retVal.mat[3][3] =
				a.mat[3][0] * b.mat[0][3] +
				a.mat[3][1] * b.mat[1][3] +
				a.mat[3][2] * b.mat[2][3] +
				a.mat[3][3] * b.mat[3][3];

			return retVal;
		}

		FTMatrix4& operator*=(const FTMatrix4& right)
		{
			*this = *this * right;
			return *this;
		}

		// Invert the matrix - super slow
		void Invert();

		void	  Transpose();
		FTMatrix4 Transposed() const;

		// Get the translation component of the matrix
		FTVector3 GetTranslation() const
		{
			return FTVector3(mat[3][0], mat[3][1], mat[3][2]);
		}

		// Get the x axis of the matrix (forward)
		FTVector3 GetXAxis() const
		{
			return FTVector3::Normalize(FTVector3(mat[0][0], mat[0][1], mat[0][2]));
		}

		// Get the y axis of the matrix (left)
		FTVector3 GetYAxis() const
		{
			return FTVector3::Normalize(FTVector3(mat[1][0], mat[1][1], mat[1][2]));
		}

		// Get the Z axis of the matrix (up)
		FTVector3 GetZAxis() const
		{
			return FTVector3::Normalize(FTVector3(mat[2][0], mat[2][1], mat[2][2]));
		}

		// Extract the scale component from the matrix
		FTVector3 GetScale() const
		{
			FTVector3 retVal;
			retVal.x = FTVector3(mat[0][0], mat[0][1], mat[0][2]).Length();
			retVal.y = FTVector3(mat[1][0], mat[1][1], mat[1][2]).Length();
			retVal.z = FTVector3(mat[2][0], mat[2][1], mat[2][2]).Length();
			return retVal;
		}

		// Create a scale matrix with x, y, and z scales
		static FTMatrix4 CreateScale(float xScale, float yScale, float zScale)
		{
			float temp[4][4] = {
				{ xScale, 0.0f, 0.0f, 0.0f },
				{ 0.0f, yScale, 0.0f, 0.0f },
				{ 0.0f, 0.0f, zScale, 0.0f },
				{ 0.0f, 0.0f, 0.0f, 1.0f }
			};
			return FTMatrix4(temp);
		}

		static FTMatrix4 CreateScale(const FTVector3& scaleVector)
		{
			return CreateScale(scaleVector.x, scaleVector.y, scaleVector.z);
		}

		// Create a scale matrix with a uniform factor
		static FTMatrix4 CreateScale(float scale)
		{
			return CreateScale(scale, scale, scale);
		}

		// Rotation about x-axis
		static FTMatrix4 CreateRotationX(float theta)
		{
			float temp[4][4] = {
				{ 1.0f, 0.0f, 0.0f, 0.0f },
				{ 0.0f, Math::Cos(theta), Math::Sin(theta), 0.0f },
				{ 0.0f, -Math::Sin(theta), Math::Cos(theta), 0.0f },
				{ 0.0f, 0.0f, 0.0f, 1.0f },
			};
			return FTMatrix4(temp);
		}

		// Rotation about y-axis
		static FTMatrix4 CreateRotationY(float theta)
		{
			float temp[4][4] = {
				{ Math::Cos(theta), 0.0f, -Math::Sin(theta), 0.0f },
				{ 0.0f, 1.0f, 0.0f, 0.0f },
				{ Math::Sin(theta), 0.0f, Math::Cos(theta), 0.0f },
				{ 0.0f, 0.0f, 0.0f, 1.0f },
			};
			return FTMatrix4(temp);
		}

		// Rotation about z-axis
		static FTMatrix4 CreateRotationZ(float theta)
		{
			float temp[4][4] = {
				{ Math::Cos(theta), Math::Sin(theta), 0.0f, 0.0f },
				{ -Math::Sin(theta), Math::Cos(theta), 0.0f, 0.0f },
				{ 0.0f, 0.0f, 1.0f, 0.0f },
				{ 0.0f, 0.0f, 0.0f, 1.0f },
			};
			return FTMatrix4(temp);
		}

		// Create a rotation matrix from a quaternion
		static FTMatrix4 CreateFromQuaternion(const class Quaternion& q);

		static FTMatrix4 CreateTranslation(const FTVector3& trans)
		{
			float temp[4][4] = {
				{ 1.0f, 0.0f, 0.0f, 0.0f },
				{ 0.0f, 1.0f, 0.0f, 0.0f },
				{ 0.0f, 0.0f, 1.0f, 0.0f },
				{ trans.x, trans.y, trans.z, 1.0f }
			};
			return FTMatrix4(temp);
		}

		static FTMatrix4 CreateLookAt(const FTVector3& eye, const FTVector3& target, const FTVector3& up)
		{
			FTVector3 zaxis = FTVector3::Normalize(target - eye);
			FTVector3 xaxis = FTVector3::Normalize(FTVector3::Cross(up, zaxis));
			FTVector3 yaxis = FTVector3::Normalize(FTVector3::Cross(zaxis, xaxis));
			FTVector3 trans;
			trans.x = -FTVector3::Dot(xaxis, eye);
			trans.y = -FTVector3::Dot(yaxis, eye);
			trans.z = -FTVector3::Dot(zaxis, eye);

			float temp[4][4] = {
				{ xaxis.x, yaxis.x, zaxis.x, 0.0f },
				{ xaxis.y, yaxis.y, zaxis.y, 0.0f },
				{ xaxis.z, yaxis.z, zaxis.z, 0.0f },
				{ trans.x, trans.y, trans.z, 1.0f }
			};
			return FTMatrix4(temp);
		}

		static FTMatrix4 CreateOrtho(float width, float height, float nearZ, float farZ)
		{
			float temp[4][4] = {
				{ 2.0f / width, 0.0f, 0.0f, 0.0f },
				{ 0.0f, 2.0f / height, 0.0f, 0.0f },
				{ 0.0f, 0.0f, 1.0f / (farZ - nearZ), 0.0f },
				{ 0.0f, 0.0f, nearZ / (nearZ - farZ), 1.0f }
			};
			return FTMatrix4(temp);
		}

		static FTMatrix4 CreatePerspectiveFOV(float fovY, float width, float height, float nearZ, float farZ)
		{
			float yScale	 = Math::Cot(fovY / 2.0f);
			float xScale	 = yScale * height / width;
			float temp[4][4] = {
				{ xScale, 0.0f, 0.0f, 0.0f },
				{ 0.0f, yScale, 0.0f, 0.0f },
				{ 0.0f, 0.0f, farZ / (farZ - nearZ), 1.0f },
				{ 0.0f, 0.0f, -nearZ * farZ / (farZ - nearZ), 0.0f }
			};
			return FTMatrix4(temp);
		}

		// Create "Simple" View-Projection Matrix from Chapter 6
		static FTMatrix4 CreateSimpleViewProj(float width, float height)
		{
			float temp[4][4] = {
				{ 2.0f / width, 0.0f, 0.0f, 0.0f },
				{ 0.0f, 2.0f / height, 0.0f, 0.0f },
				{ 0.0f, 0.0f, 1.0f, 0.0f },
				{ 0.0f, 0.0f, 1.0f, 1.0f }
			};
			return FTMatrix4(temp);
		}

		static const FTMatrix4 Identity;
	};

	// (Unit) Quaternion
	class Quaternion
	{
	public:
		float x;
		float y;
		float z;
		float w;

		Quaternion()
		{
			*this = Quaternion::Identity;
		}

		// This directly sets the quaternion components --
		// don't use for axis/angle
		explicit Quaternion(float inX, float inY, float inZ, float inW)
		{
			Set(inX, inY, inZ, inW);
		}

		// Construct the quaternion from an axis and angle
		// It is assumed that axis is already normalized,
		// and the angle is in radians
		explicit Quaternion(const FTVector3& axis, float angle)
		{
			float scalar = Math::Sin(angle / 2.0f);
			x			 = axis.x * scalar;
			y			 = axis.y * scalar;
			z			 = axis.z * scalar;
			w			 = Math::Cos(angle / 2.0f);
		}

		// Directly set the internal components
		void Set(float inX, float inY, float inZ, float inW)
		{
			x = inX;
			y = inY;
			z = inZ;
			w = inW;
		}

		void Conjugate()
		{
			x *= -1.0f;
			y *= -1.0f;
			z *= -1.0f;
		}

		float LengthSq() const
		{
			return (x * x + y * y + z * z + w * w);
		}

		float Length() const
		{
			return Math::Sqrt(LengthSq());
		}

		void Normalize()
		{
			float length = Length();
			x /= length;
			y /= length;
			z /= length;
			w /= length;
		}

		// Normalize the provided quaternion
		static Quaternion Normalize(const Quaternion& q)
		{
			Quaternion retVal = q;
			retVal.Normalize();
			return retVal;
		}

		// Linear interpolation
		static Quaternion Lerp(const Quaternion& a, const Quaternion& b, float f)
		{
			Quaternion retVal;
			retVal.x = Math::Lerp(a.x, b.x, f);
			retVal.y = Math::Lerp(a.y, b.y, f);
			retVal.z = Math::Lerp(a.z, b.z, f);
			retVal.w = Math::Lerp(a.w, b.w, f);
			retVal.Normalize();
			return retVal;
		}

		static float Dot(const Quaternion& a, const Quaternion& b)
		{
			return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
		}

		// Spherical Linear Interpolation
		static Quaternion Slerp(const Quaternion& a, const Quaternion& b, float f)
		{
			float rawCosm = Quaternion::Dot(a, b);

			float cosom = -rawCosm;
			if (rawCosm >= 0.0f)
			{
				cosom = rawCosm;
			}

			float scale0, scale1;

			if (cosom < 0.9999f)
			{
				const float omega  = Math::Acos(cosom);
				const float invSin = 1.f / Math::Sin(omega);
				scale0			   = Math::Sin((1.f - f) * omega) * invSin;
				scale1			   = Math::Sin(f * omega) * invSin;
			}
			else
			{
				// Use linear interpolation if the quaternions
				// are collinear
				scale0 = 1.0f - f;
				scale1 = f;
			}

			if (rawCosm < 0.0f)
			{
				scale1 = -scale1;
			}

			Quaternion retVal;
			retVal.x = scale0 * a.x + scale1 * b.x;
			retVal.y = scale0 * a.y + scale1 * b.y;
			retVal.z = scale0 * a.z + scale1 * b.z;
			retVal.w = scale0 * a.w + scale1 * b.w;
			retVal.Normalize();
			return retVal;
		}

		// Concatenate
		// Rotate by q FOLLOWED BY p
		static Quaternion Concatenate(const Quaternion& q, const Quaternion& p)
		{
			Quaternion retVal;

			// Vector component is:
			// ps * qv + qs * pv + pv x qv
			FTVector3 qv(q.x, q.y, q.z);
			FTVector3 pv(p.x, p.y, p.z);
			FTVector3 newVec = p.w * qv + q.w * pv + FTVector3::Cross(pv, qv);
			retVal.x		 = newVec.x;
			retVal.y		 = newVec.y;
			retVal.z		 = newVec.z;

			// Scalar component is:
			// ps * qs - pv . qv
			retVal.w = p.w * q.w - FTVector3::Dot(pv, qv);

			return retVal;
		}

		static const Quaternion Identity;
	};

	inline bool PointInRectangle(FTVector2 point, FTVector2 v0, FTVector2 v1, FTVector2 v2, FTVector2 v3)
	{
		FTVector2 AB	  = v1 - v0;
		FTVector2 AM	  = point - v0;
		FTVector2 BC	  = v2 - v1;
		FTVector2 BM	  = point - v1;
		float	  dotABAM = FTVector2::Dot(AB, AM);
		float	  dotABAB = FTVector2::Dot(AB, AB);
		float	  dotBCBM = FTVector2::Dot(BC, BM);
		float	  dotBCBC = FTVector2::Dot(BC, BC);
		return 0 <= dotABAM && dotABAM <= dotABAB && 0 <= dotBCBM && dotBCBM <= dotBCBC;
	}
} // namespace Math