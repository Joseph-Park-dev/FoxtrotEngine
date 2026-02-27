// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
//
// Released under the BSD License
// https://github.com/gameprogcpp/code
// ----------------------------------------------------------------
// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2026 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Math related classes, especially Vectors and their operations.
/// </summary>

#pragma once
#include <cmath>
#include <limits>
#include <assert.h>
#include <directxmath.h>
#include <box2d/box2d.h>

#ifdef FOXTROT_EDITOR
	#define IMGUI_DEFINE_MATH_OPERATORS
	#include <../FoxtrotEditor/include/imgui/imgui.h>
#endif // FOXTROT_EDITOR

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

	// inline DirectX::SimpleMath::Vector3 QuaternionToEuler(const DirectX::SimpleMath::Quaternion& q)
	//{
	//	// Roll (x-axis rotation)
	//	float sinr_cosp = 2.f * (q.w * q.x + q.y * q.z);
	//	float cosr_cosp = 1.f - 2.f * (q.x * q.x + q.y * q.y);
	//	float roll		= std::atan2(sinr_cosp, cosr_cosp);

	//	// Pitch (y-axis rotation)
	//	float sinp = 2.f * (q.w * q.y - q.z * q.x);
	//	float pitch;
	//	// Clamp sinp to the range [-1, 1] to account for numerical errors that might push it out of range.
	//	if (std::fabs(sinp) >= 1.f)
	//		pitch = std::copysign((float)DirectX::XM_PI / 2.f, sinp);
	//	else
	//		pitch = std::asin(sinp);

	//	// Yaw (z-axis rotation)
	//	float siny_cosp = 2.f * (q.w * q.z + q.x * q.y);
	//	float cosy_cosp = 1.f - 2.f * (q.y * q.y + q.z * q.z);
	//	float yaw		= std::atan2(siny_cosp, cosy_cosp);

	//	return DirectX::SimpleMath::Vector3(roll, pitch, yaw);
	//}

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

	using FTVector2	 = DirectX::XMFLOAT2;
	using FTVector3	 = DirectX::XMFLOAT3;
	using FTVector4	 = DirectX::XMFLOAT4;
	using FTLocalVec = DirectX::XMVECTOR;

	/// @brief Only using this for a single operator+ call is recommended.
	/// For a chain of operations, use DirectX::XMVectorAdd() instead.
	FTVector3 operator+(const FTVector3& a, const FTVector3& b)
	{
		return { a.x + b.x, a.y + b.y, a.z + b.z };
	}

	/// @brief Only using this for a single operator- call is recommended.
	/// For a chain of operations, use DirectX::XMVectorSubtract() instead.
	FTVector3 operator-(const FTVector3& a, const FTVector3& b)
	{
		return { a.x - b.x, a.y - b.y, a.z - b.z };
	}

	/// @brief Only using this for a single operator* call is recommended.
	/// For a chain of operations, use DirectX::XMVectorMultiply() instead.
	FTVector3 operator*(const FTVector3& left, const FTVector3& right)
	{
		return { left.x * right.x, left.y * right.y, left.z * right.z };
	}

	/// @brief Only using this for a single operator* call is recommended.
	/// For a chain of operations, use DirectX::XMVectorScale() instead.
	FTVector3 operator*(const FTVector3& vec, float scalar)
	{
		return { vec.x * scalar, vec.y * scalar, vec.z * scalar };
	}

	/// @brief Only using this for a single operator* call is recommended.
	/// For a chain of operations, use DirectX::XMVectorScale() instead.
	FTVector3 operator*(float scalar, const FTVector3& vec)
	{
		return { vec.x * scalar, vec.y * scalar, vec.z * scalar };
	}

	/// @brief Only using this for a single operator/ call is recommended.
	/// For a chain of operations, use DirectX::XMVectorScale() instead.
	FTVector3 operator/(const FTVector3& vec, float scalar)
	{
		assert(scalar != 0);
		return { vec.x / scalar, vec.y / scalar, vec.z / scalar };
	}

	bool operator==(const FTVector3& left, const FTVector3& right)
	{
		return (left.x == right.x) && (left.y == right.y) && (left.z == right.z);
	}

	bool operator!=(const FTVector3& left, const FTVector3& right)
	{
		return (left.x != right.x) || (left.y != right.y) || (left.z != right.z);
	}

	/// @brief Only using this for a single operator* call is recommended.
	/// For a chain of operations, use DirectX::XMVectorScale() instead.
	void operator*=(FTVector3& vec3, float scalar)
	{
		vec3.x *= scalar;
		vec3.y *= scalar;
		vec3.z *= scalar;
	}

	/// @brief Only using this for a single operator+ call is recommended.
	/// For a chain of operations, use DirectX::XMVectorAdd() instead.
	void operator+=(FTVector3& left, const FTVector3& right)
	{
		left.x += right.x;
		left.y += right.y;
		left.z += right.z;
	}

	/// @brief Only using this for a single operator- call is recommended.
	/// For a chain of operations, use DirectX::XMVectorSubtract() instead.
	void operator-=(FTVector3& left, const FTVector3& right)
	{
		left.x -= right.x;
		left.y -= right.y;
		left.z -= right.z;
	}

	/// @brief Only using this for a single operator* call is recommended.
	/// For a chain of operations, use DirectX::XMVectorMultiply() instead.
	void operator*=(FTVector3& left, const FTVector3& right)
	{
		left.x *= right.x;
		left.y *= right.y;
		left.z *= right.z;
	}

	/// @brief Only using this for a single operator/ call is recommended.
	/// For a chain of operations, use DirectX::XMVectorScale() instead.
	void operator/=(FTVector3& left, const float& right)
	{
		left.x /= right;
		left.y /= right;
		left.z /= right;
	}

	// FTVector3 void DecomposeMatrix(FTVector3& scale, FTVector3& rot, FTVector3& pos, DirectX::SimpleMath::Matrix& matrix);

#ifdef FOXTROT_EDITOR
	/// @brief Only using this for a single operator- call is recommended.
	/// For a chain of operations, use DirectX::XMVectorSubtract() instead.
	void operator-=(FTVector3& left, const ImVec2& right)
	{
		left.x -= right.x;
		left.y -= right.y;
	}
#endif

	/// @brief Only using this for a single operator+ call is recommended.
	/// For a chain of operations, use DirectX::XMVectorAdd() instead.
	FTVector2 operator+(const FTVector2& a, const FTVector2& b)
	{
		return { a.x + b.x, a.y + b.y };
	}

	/// @brief Only using this for a single operator- call is recommended.
	/// For a chain of operations, use DirectX::XMVectorSubtract() instead.
	FTVector2 operator-(const FTVector2& a, const FTVector2& b)
	{
		return { a.x - b.x, a.y - b.y };
	}

	/// @brief Only using this for a single operator* call is recommended.
	/// For a chain of operations, use DirectX::XMVectorMultiply() instead.
	FTVector2 operator*(const FTVector2& a, const FTVector2& b)
	{
		return { a.x * b.x, a.y * b.y };
	}

	/// @brief Only using this for a single operator* call is recommended.
	/// For a chain of operations, use DirectX::XMVectorScale() instead.
	FTVector2 operator*(const FTVector2& vec, float scalar)
	{
		return { vec.x * scalar, vec.y * scalar };
	}

	/// @brief Only using this for a single operator* call is recommended.
	/// For a chain of operations, use DirectX::XMVectorScale() instead.
	FTVector2 operator*(float scalar, const FTVector2& vec)
	{
		return { vec.x * scalar, vec.y * scalar };
	}

	/// @brief Only using this for a single operator* call is recommended.
	/// For a chain of operations, use DirectX::XMVectorScale() instead.
	FTVector2 operator/(FTVector2 left, FTVector2 right)
	{
		assert(!(right.x == 0.f || right.y == 0.f));
		return { left.x / right.x, left.y / right.y };
	}

	/// @brief Only using this for a single operator/ call is recommended.
	/// For a chain of operations, use DirectX::XMVectorScale() instead.
	FTVector2 operator/(FTVector2 vec, float scalar)
	{
		assert(!(scalar == 0.f));
		return { vec.x / scalar, vec.y / scalar };
	}

	/// @brief Only using this for a single operator/ call is recommended.
	/// For a chain of operations, use DirectX::XMVectorScale() instead.
	void operator/=(FTVector2& vec, float scalar)
	{
		assert(!(scalar == 0.f));
		vec.x /= scalar;
		vec.y /= scalar;
	}

	/// @brief Only using this for a single operator*= call is recommended.
	/// For a chain of operations, use DirectX::XMVectorScale() instead.
	void operator*=(FTVector2& vec, float scalar)
	{
		vec.x *= scalar;
		vec.y *= scalar;
	}

	/// @brief Only using this for a single operator*= call is recommended.
	/// For a chain of operations, use DirectX::XMVectorMultiply() instead.
	void operator*=(FTVector2& left, FTVector2 right)
	{
		left.x *= right.x;
		left.y *= right.y;
	}

	/// @brief Only using this for a single operator+= call is recommended.
	/// For a chain of operations, use DirectX::XMVectorAdd() instead.
	void operator+=(FTVector2& left, FTVector2 right)
	{
		left.x += right.x;
		left.y += right.y;
	}

	/// @brief Only using this for a single operator-= call is recommended.
	/// For a chain of operations, use DirectX::XMVectorSubtract() instead.
	void operator-=(FTVector2& left, const FTVector2& right)
	{
		left.x -= right.x;
		left.y -= right.y;
	}

	/// @brief Only using this for a single operator- call is recommended.
	/// For a chain of operations, use DirectX::XMVectorSubtract() instead.
	void operator-(FTVector2& val)
	{
		val.x = -val.x;
		val.y = -val.y;
	}

	inline bool PointInRectangle(FTVector2* point, FTVector2* vert0, FTVector2* vert1, FTVector2* vert2, FTVector2* vert3)
	{
		using namespace DirectX;
		FTLocalVec p  = XMLoadFloat2(point);
		FTLocalVec v0 = XMLoadFloat2(vert0);
		FTLocalVec v1 = XMLoadFloat2(vert1);
		FTLocalVec v2 = XMLoadFloat2(vert2);
		FTLocalVec v3 = XMLoadFloat2(vert3);

		FTLocalVec AB = XMVectorSubtract(v1, v0);
		FTLocalVec AM = XMVectorSubtract(p, v0);
		FTLocalVec BC = XMVectorSubtract(v2, v1);
		FTLocalVec BM = XMVectorSubtract(p, v1);

		float dotABAM = XMVectorGetX(XMVector2Dot(AB, AM));
		float dotABAB = XMVectorGetX(XMVector2Dot(AB, AB));
		float dotBCBM = XMVectorGetX(XMVector2Dot(BC, BM));
		float dotBCBC = XMVectorGetX(XMVector2Dot(BC, BC));
		return 0 <= dotABAM && dotABAM <= dotABAB && 0 <= dotBCBM && dotBCBM <= dotBCBC;
	}
} // namespace Math