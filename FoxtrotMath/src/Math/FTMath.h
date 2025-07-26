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

#ifdef DLL_EXPORT
	#define FOXTROT_API __declspec(dllexport)
#else
	#define FOXTROT_API __declspec(dllimport)
#endif

#include <cmath>
#include <memory.h>
#include <limits>
#include <assert.h>
#include <directxmath.h>
#include <fstream>
#include <string>

#include "box2d/box2d.h"
#include "directxtk/SimpleMath.h"

#ifdef FOXTROT_EDITOR
	#define IMGUI_DEFINE_MATH_OPERATORS
	#include <imgui.h>
#endif // FOXTROT_EDITOR

extern "C"
{
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

		inline DirectX::SimpleMath::Vector3 QuaternionToEuler(const DirectX::SimpleMath::Quaternion& q)
		{
			// Roll (x-axis rotation)
			float sinr_cosp = 2.f * (q.w * q.x + q.y * q.z);
			float cosr_cosp = 1.f - 2.f * (q.x * q.x + q.y * q.y);
			float roll = std::atan2(sinr_cosp, cosr_cosp);

			// Pitch (y-axis rotation)
			float sinp = 2.f * (q.w * q.y - q.z * q.x);
			float pitch;
			// Clamp sinp to the range [-1, 1] to account for numerical errors that might push it out of range.
			if (std::fabs(sinp) >= 1.f)
				pitch = std::copysign((float)DirectX::XM_PI / 2.f, sinp);
			else
				pitch = std::asin(sinp);

			// Yaw (z-axis rotation)
			float siny_cosp = 2.f * (q.w * q.z + q.x * q.y);
			float cosy_cosp = 1.f - 2.f * (q.y * q.y + q.z * q.z);
			float yaw = std::atan2(siny_cosp, cosy_cosp);

			return DirectX::SimpleMath::Vector3(roll, pitch, yaw);
		}

		inline size_t NextPowerOf2(int m)
		{
			m--;
			m |= m >> 1;
			m |= m >> 2;
			m |= m >> 4;
			m |= m >> 8;
			m |= m >> 16;
			m |= m >> 32;
			m++;
			return m;
		}
	} // namespace Math
}

class FTVector2;

// 3D Vector
class FOXTROT_API FTVector3
{
public:
	float x;
	float y;
	float z;

	FTVector3();
	explicit FTVector3(float in);
	explicit FTVector3(float inX, float inY, float inZ);

	FTVector3(FTVector2 vec2);
	FTVector3(b2Vec2 vec2);
	FTVector3(DirectX::SimpleMath::Vector3 vec3);

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

	// Vector /=
	friend FTVector3 operator/=(FTVector3& left, const float& right)
	{
		left.x /= right;
		left.y /= right;
		left.z /= right;
		return left;
	}

	static void DecomposeMatrix(FTVector3& scale, FTVector3& rot, FTVector3& pos, DirectX::SimpleMath::Matrix& matrix);

	friend std::ofstream& operator<<(std::ofstream& ofs, const FTVector3& vec3)
	{
		ofs << "(" << std::to_string(vec3.x) << "," << std::to_string(vec3.y) << "," << std::to_string(vec3.z) << ")";
		return ofs;
	}

	const b2Vec2			GetB2Vec2() const;
	const DirectX::XMFLOAT3 GetDXVec3() const;

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

// 2D Vector
class FOXTROT_API FTVector2
{
public:
	float x;
	float y;

	FTVector2()
		: x(0.0f)
		, y(0.0f)
	{
	}

	explicit FTVector2(float inX, float inY)
		: x(inX)
		, y(inY)
	{
	}

	explicit FTVector2(int inX, int inY)
		: x(static_cast<float>(inX))
		, y(static_cast<float>(inY))
	{
	}

	explicit FTVector2(b2Vec2 vec2)
		: x(static_cast<float>(vec2.x))
		, y(static_cast<float>(vec2.y))
	{
	}

#ifdef FOXTROT_EDITOR
	explicit FTVector2(ImVec2 imguiVec2)
		: x(imguiVec2.x)
		, y(imguiVec2.y)
	{
	}

	ImVec2 GetImVec2()
	{
		return ImVec2(this->x, this->y);
	}

	FTVector2 operator-=(const ImVec2& imguiVec2)
	{
		x -= imguiVec2.x;
		y -= imguiVec2.y;
		return *this;
	}
#endif // FOXTROT_EDITOR

	FTVector2(FTVector3 vec3)
		: x(vec3.x)
		, y(vec3.y)
	{
	}

	friend std::ofstream& operator<<(std::ofstream& ofs, const FTVector2& vec2)
	{
		ofs << "(" << std::to_string(vec2.x) << "," << std::to_string(vec2.y) << ")";
		return ofs;
	}

	DirectX::XMFLOAT2 GetD3Vec2()
	{
		return DirectX::XMFLOAT2(this->x, this->y);
	}

	b2Vec2 GetB2Vec2()
	{
		return b2Vec2{ this->x, this->y };
	}

	// Set both components in one line
	void Set(float inX, float inY)
	{
		x = inX;
		y = inY;
	}

	// Vector addition (a + b)
	friend FTVector2 operator+(const FTVector2& a, const FTVector2& b)
	{
		return FTVector2(a.x + b.x, a.y + b.y);
	}

	// Vector addition (a + b)
	friend FTVector2 operator+(const FTVector2& a, const FTVector3& b)
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

	FTVector2 operator=(FTVector3& vec3)
	{
		return FTVector2(vec3.x, vec3.y);
	}

	FTVector2 operator=(const FTVector3& vec3)
	{
		return FTVector2(vec3.x, vec3.y);
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

	// Transform vector by matrix
	static FTVector2 Transform(const FTVector2& vec, const class Matrix3& mat, float w = 1.0f);

	static FTVector2 CubicBezierVelocity2D(const FTVector2& P0, const FTVector2& P1, const FTVector2& P2, const FTVector2& P3, double t);
	static FTVector2 PlotCircularFall(float angle, float dist);

	static const FTVector2 Zero;
	static const FTVector2 UnitX;
	static const FTVector2 UnitY;
	static const FTVector2 NegUnitX;
	static const FTVector2 NegUnitY;
};

extern "C"
{
	namespace Math
	{
		inline bool PointInRectangle(FTVector2 point, FTVector2 v0, FTVector2 v1, FTVector2 v2, FTVector2 v3)
		{
			FTVector2 AB = v1 - v0;
			FTVector2 AM = point - v0;
			FTVector2 BC = v2 - v1;
			FTVector2 BM = point - v1;
			float	  dotABAM = FTVector2::Dot(AB, AM);
			float	  dotABAB = FTVector2::Dot(AB, AB);
			float	  dotBCBM = FTVector2::Dot(BC, BM);
			float	  dotBCBC = FTVector2::Dot(BC, BC);
			return 0 <= dotABAM && dotABAM <= dotABAB && 0 <= dotBCBM && dotBCBM <= dotBCBC;
		}
	} // namespace Math
}