// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once

#include <cmath>
#include <memory.h>
#include <limits>
#include <assert.h>
#include <directxmath.h>

namespace Math
{
	const float Pi = 3.1415926535f;
	const float TwoPi = Pi * 2.0f;
	const float PiOver2 = Pi / 2.0f;
	const float Infinity = std::numeric_limits<float>::infinity();
	const float NegInfinity = -std::numeric_limits<float>::infinity();

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

	template <typename T>
	T Max(const T& a, const T& b)
	{
		return (a < b ? b : a);
	}

	template <typename T>
	T Min(const T& a, const T& b)
	{
		return (a < b ? a : b);
	}

	template <typename T>
	T Clamp(const T& value, const T& lower, const T& upper)
	{
		return Min(upper, Max(lower, value));
	}

	inline float Abs(float value)
	{
		return fabs(value);
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
}

// 2D Vector
class FTVector2
{
public:
	float x;
	float y;

	FTVector2()
		:x(0.0f)
		,y(0.0f)
	{}

	explicit FTVector2(float inX, float inY)
		:x(inX)
		,y(inY)
	{}

	explicit FTVector2(int inX, int inY)
		:x(static_cast<float>(inX))
		,y(static_cast<float>(inY))
	{}

	DirectX::XMFLOAT2 GetD3Vec2()
	{
		return DirectX::XMFLOAT2(this->x, this->y);
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

	FTVector2 operator / (FTVector2 vOther)
	{
		assert(!(vOther.x == 0.f || vOther.y == 0.f));
		return FTVector2(x / vOther.x, y / vOther.y);
	}

	FTVector2 operator / (float _f)
	{
		assert(!(_f == 0.f));
		return FTVector2(x / _f, y / _f);
	}

	// Scalar *=
	FTVector2& operator*=(float scalar)
	{
		x *= scalar;
		y *= scalar;
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
		return FTVector2(-x,-y);
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
		return (x*x + y*y);
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

	static const FTVector2 Zero;
	static const FTVector2 UnitX;
	static const FTVector2 UnitY;
	static const FTVector2 NegUnitX;
	static const FTVector2 NegUnitY;
};

// 3D Vector
class FTVector3
{
public:
	float x;
	float y;
	float z;

	FTVector3()
		:x(0.0f)
		,y(0.0f)
		,z(0.0f)
	{}

	explicit FTVector3(float inX, float inY, float inZ)
		:x(inX)
		,y(inY)
		,z(inZ)
	{}

	DirectX::XMFLOAT3 GetD3Vec2()
	{
		return DirectX::XMFLOAT3(this->x, this->y, this->z);
	}

	// Cast to a const float pointer
	const float* GetAsFloatPtr() const
	{
		return reinterpret_cast<const float*>(&x);
	}

	// Set all three components in one line
	void Set(float inX, float inY, float inZ)
	{
		x = inX;
		y = inY;
		z = inZ;
	}

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

	friend FTVector3 operator/(float scalar, const FTVector3& vec)
	{
		assert(scalar != 0);
		return FTVector3(vec.x / scalar, vec.y / scalar, vec.z / scalar);
	}

	// Scalar *=
	FTVector3& operator*=(float scalar)
	{
		x *= scalar;
		y *= scalar;
		z *= scalar;
		return *this;
	}

	// Vector +=
	FTVector3& operator+=(const FTVector3& right)
	{
		x += right.x;
		y += right.y;
		z += right.z;
		return *this;
	}

	// Vector -=
	FTVector3& operator-=(const FTVector3& right)
	{
		x -= right.x;
		y -= right.y;
		z -= right.z;
		return *this;
	}

	// Length squared of vector
	float LengthSq() const
	{
		return (x*x + y*y + z*z);
	}

	// Length of vector
	float Length() const
	{
		return (Math::Sqrt(LengthSq()));
	}

	// Normalize this vector
	void Normalize()
	{
		float length = Length();
		x /= length;
		y /= length;
		z /= length;
	}

	// Normalize the provided vector
	static FTVector3 Normalize(const FTVector3& vec)
	{
		FTVector3 temp = vec;
		temp.Normalize();
		return temp;
	}

	// Dot product between two vectors (a dot b)
	static float Dot(const FTVector3& a, const FTVector3& b)
	{
		return (a.x * b.x + a.y * b.y + a.z * b.z);
	}

	// Cross product between two vectors (a cross b)
	static FTVector3 Cross(const FTVector3& a, const FTVector3& b)
	{
		FTVector3 temp;
		temp.x = a.y * b.z - a.z * b.y;
		temp.y = a.z * b.x - a.x * b.z;
		temp.z = a.x * b.y - a.y * b.x;
		return temp;
	}

	// Lerp from A to B by f
	static FTVector3 Lerp(const FTVector3& a, const FTVector3& b, float f)
	{
		return FTVector3(a + f * (b - a));
	}
	
	// Reflect V about (normalized) N
	static FTVector3 Reflect(const FTVector3& v, const FTVector3& n)
	{
		return v - 2.0f * FTVector3::Dot(v, n) * n;
	}

	static FTVector3 Transform(const FTVector3& vec, const class Matrix4& mat, float w = 1.0f);
	// This will transform the vector and renormalize the w component
	static FTVector3 TransformWithPerspDiv(const FTVector3& vec, const class Matrix4& mat, float w = 1.0f);

	// Transform a FTVector3 by a quaternion
	static FTVector3 Transform(const FTVector3& v, const class FTQuaternion& q);

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

// 3x3 Matrix
class Matrix3
{
public:
	float mat[3][3];

	Matrix3()
	{
		*this = Matrix3::Identity;
	}

	explicit Matrix3(float inMat[3][3])
	{
		memcpy(mat, inMat, 9 * sizeof(float));
	}

	// Cast to a const float pointer
	const float* GetAsFloatPtr() const
	{
		return reinterpret_cast<const float*>(&mat[0][0]);
	}

	// Matrix multiplication
	friend Matrix3 operator*(const Matrix3& left, const Matrix3& right)
	{
		Matrix3 retVal;
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

	Matrix3& operator*=(const Matrix3& right)
	{
		*this = *this * right;
		return *this;
	}

	// Create a scale matrix with x and y scales
	static Matrix3 CreateScale(float xScale, float yScale)
	{
		float temp[3][3] =
		{
			{ xScale, 0.0f, 0.0f },
			{ 0.0f, yScale, 0.0f },
			{ 0.0f, 0.0f, 1.0f },
		};
		return Matrix3(temp);
	}

	static Matrix3 CreateScale(const FTVector2& scaleVector)
	{
		return CreateScale(scaleVector.x, scaleVector.y);
	}

	// Create a scale matrix with a uniform factor
	static Matrix3 CreateScale(float scale)
	{
		return CreateScale(scale, scale);
	}

	// Create a rotation matrix about the Z axis
	// theta is in radians
	static Matrix3 CreateRotation(float theta)
	{
		float temp[3][3] =
		{
			{ Math::Cos(theta), Math::Sin(theta), 0.0f },
			{ -Math::Sin(theta), Math::Cos(theta), 0.0f },
			{ 0.0f, 0.0f, 1.0f },
		};
		return Matrix3(temp);
	}

	// Create a translation matrix (on the xy-plane)
	static Matrix3 CreateTranslation(const FTVector2& trans)
	{
		float temp[3][3] =
		{
			{ 1.0f, 0.0f, 0.0f },
			{ 0.0f, 1.0f, 0.0f },
			{ trans.x, trans.y, 1.0f },
		};
		return Matrix3(temp);
	}

	static const Matrix3 Identity;
};

// 4x4 Matrix
class Matrix4
{
public:
	float mat[4][4];

	Matrix4()
	{
		*this = Matrix4::Identity;
	}

	explicit Matrix4(float inMat[4][4])
	{
		memcpy(mat, inMat, 16 * sizeof(float));
	}

	// Cast to a const float pointer
	const float* GetAsFloatPtr() const
	{
		return reinterpret_cast<const float*>(&mat[0][0]);
	}

	// Matrix multiplication (a * b)
	friend Matrix4 operator*(const Matrix4& a, const Matrix4& b)
	{
		Matrix4 retVal;
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

	Matrix4& operator*=(const Matrix4& right)
	{
		*this = *this * right;
		return *this;
	}

	// Invert the matrix - super slow
	void Invert();

	// Get the translation component of the matrix
	FTVector3 GetTranslation() const
	{
		return FTVector3(mat[3][0], mat[3][1], mat[3][2]);
	}
	
	// Get the X axis of the matrix (forward)
	FTVector3 GetXAxis() const
	{
		return FTVector3::Normalize(FTVector3(mat[0][0], mat[0][1], mat[0][2]));
	}

	// Get the Y axis of the matrix (left)
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
	static Matrix4 CreateScale(float xScale, float yScale, float zScale)
	{
		float temp[4][4] =
		{
			{ xScale, 0.0f, 0.0f, 0.0f },
			{ 0.0f, yScale, 0.0f, 0.0f },
			{ 0.0f, 0.0f, zScale, 0.0f },
			{ 0.0f, 0.0f, 0.0f, 1.0f }
		};
		return Matrix4(temp);
	}

	static Matrix4 CreateScale(const FTVector3& scaleVector)
	{
		return CreateScale(scaleVector.x, scaleVector.y, scaleVector.z);
	}

	// Create a scale matrix with a uniform factor
	static Matrix4 CreateScale(float scale)
	{
		return CreateScale(scale, scale, scale);
	}

	// Rotation about x-axis
	static Matrix4 CreateRotationX(float theta)
	{
		float temp[4][4] =
		{
			{ 1.0f, 0.0f, 0.0f , 0.0f },
			{ 0.0f, Math::Cos(theta), Math::Sin(theta), 0.0f },
			{ 0.0f, -Math::Sin(theta), Math::Cos(theta), 0.0f },
			{ 0.0f, 0.0f, 0.0f, 1.0f },
		};
		return Matrix4(temp);
	}

	// Rotation about y-axis
	static Matrix4 CreateRotationY(float theta)
	{
		float temp[4][4] =
		{
			{ Math::Cos(theta), 0.0f, -Math::Sin(theta), 0.0f },
			{ 0.0f, 1.0f, 0.0f, 0.0f },
			{ Math::Sin(theta), 0.0f, Math::Cos(theta), 0.0f },
			{ 0.0f, 0.0f, 0.0f, 1.0f },
		};
		return Matrix4(temp);
	}

	// Rotation about z-axis
	static Matrix4 CreateRotationZ(float theta)
	{
		float temp[4][4] =
		{
			{ Math::Cos(theta), Math::Sin(theta), 0.0f, 0.0f },
			{ -Math::Sin(theta), Math::Cos(theta), 0.0f, 0.0f },
			{ 0.0f, 0.0f, 1.0f, 0.0f },
			{ 0.0f, 0.0f, 0.0f, 1.0f },
		};
		return Matrix4(temp);
	}

	// Create a rotation matrix from a quaternion
	static Matrix4 CreateFromQuaternion(const class FTQuaternion& q);

	static Matrix4 CreateTranslation(const FTVector3& trans)
	{
		float temp[4][4] =
		{
			{ 1.0f, 0.0f, 0.0f, 0.0f },
			{ 0.0f, 1.0f, 0.0f, 0.0f },
			{ 0.0f, 0.0f, 1.0f, 0.0f },
			{ trans.x, trans.y, trans.z, 1.0f }
		};
		return Matrix4(temp);
	}

	static Matrix4 CreateLookAt(const FTVector3& eye, const FTVector3& target, const FTVector3& up)
	{
		FTVector3 zaxis = FTVector3::Normalize(target - eye);
		FTVector3 xaxis = FTVector3::Normalize(FTVector3::Cross(up, zaxis));
		FTVector3 yaxis = FTVector3::Normalize(FTVector3::Cross(zaxis, xaxis));
		FTVector3 trans;
		trans.x = -FTVector3::Dot(xaxis, eye);
		trans.y = -FTVector3::Dot(yaxis, eye);
		trans.z = -FTVector3::Dot(zaxis, eye);

		float temp[4][4] =
		{
			{ xaxis.x, yaxis.x, zaxis.x, 0.0f },
			{ xaxis.y, yaxis.y, zaxis.y, 0.0f },
			{ xaxis.z, yaxis.z, zaxis.z, 0.0f },
			{ trans.x, trans.y, trans.z, 1.0f }
		};
		return Matrix4(temp);
	}

	//static Matrix4 CreateOrtho(float width, float height, float near, float far)
	//{
	//	float temp[4][4] =
	//	{
	//		{ 2.0f / width, 0.0f, 0.0f, 0.0f },
	//		{ 0.0f, 2.0f / height, 0.0f, 0.0f },
	//		{ 0.0f, 0.0f, 1.0f / (far - near), 0.0f },
	//		{ 0.0f, 0.0f, near / (near - far), 1.0f }
	//	};
	//	return Matrix4(temp);
	//}

	//static Matrix4 CreatePerspectiveFOV(float fovY, float width, float height, float near, float far)
	//{
	//	float yScale = Math::Cot(fovY / 2.0f);
	//	float xScale = yScale * height / width;
	//	float temp[4][4] =
	//	{
	//		{ xScale, 0.0f, 0.0f, 0.0f },
	//		{ 0.0f, yScale, 0.0f, 0.0f },
	//		{ 0.0f, 0.0f, far / (far - near), 1.0f },
	//		{ 0.0f, 0.0f, -near * far / (far - near), 0.0f }
	//	};
	//	return Matrix4(temp);
	//}

	// Create "Simple" View-Projection Matrix from Chapter 6
	static Matrix4 CreateSimpleViewProj(float width, float height)
	{
		float temp[4][4] =
		{
			{ 2.0f/width, 0.0f, 0.0f, 0.0f },
			{ 0.0f, 2.0f/height, 0.0f, 0.0f },
			{ 0.0f, 0.0f, 1.0f, 0.0f },
			{ 0.0f, 0.0f, 1.0f, 1.0f }
		};
		return Matrix4(temp);
	}
	
	static const Matrix4 Identity;
};

// (Unit) FTQuaternion
class FTQuaternion
{
public:
	float x;
	float y;
	float z;
	float w;

	FTQuaternion()
	{
		*this = FTQuaternion::Identity;
	}

	// This directly sets the quaternion components --
	// don't use for axis/angle
	explicit FTQuaternion(float inX, float inY, float inZ, float inW)
	{
		Set(inX, inY, inZ, inW);
	}

	// Construct the quaternion from an axis and angle
	// It is assumed that axis is already normalized,
	// and the angle is in radians
	explicit FTQuaternion(const FTVector3& axis, float angle)
	{
		float scalar = Math::Sin(angle / 2.0f);
		x = axis.x * scalar;
		y = axis.y * scalar;
		z = axis.z * scalar;
		w = Math::Cos(angle / 2.0f);
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
		return (x*x + y*y + z*z + w*w);
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
	static FTQuaternion Normalize(const FTQuaternion& q)
	{
		FTQuaternion retVal = q;
		retVal.Normalize();
		return retVal;
	}

	// Linear interpolation
	static FTQuaternion Lerp(const FTQuaternion& a, const FTQuaternion& b, float f)
	{
		FTQuaternion retVal;
		retVal.x = Math::Lerp(a.x, b.x, f);
		retVal.y = Math::Lerp(a.y, b.y, f);
		retVal.z = Math::Lerp(a.z, b.z, f);
		retVal.w = Math::Lerp(a.w, b.w, f);
		retVal.Normalize();
		return retVal;
	}

	static float Dot(const FTQuaternion& a, const FTQuaternion& b)
	{
		return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
	}

	// Spherical Linear Interpolation
	static FTQuaternion Slerp(const FTQuaternion& a, const FTQuaternion& b, float f)
	{
		float rawCosm = FTQuaternion::Dot(a, b);

		float cosom = -rawCosm;
		if (rawCosm >= 0.0f)
		{
			cosom = rawCosm;
		}

		float scale0, scale1;

		if (cosom < 0.9999f)
		{
			const float omega = Math::Acos(cosom);
			const float invSin = 1.f / Math::Sin(omega);
			scale0 = Math::Sin((1.f - f) * omega) * invSin;
			scale1 = Math::Sin(f * omega) * invSin;
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

		FTQuaternion retVal;
		retVal.x = scale0 * a.x + scale1 * b.x;
		retVal.y = scale0 * a.y + scale1 * b.y;
		retVal.z = scale0 * a.z + scale1 * b.z;
		retVal.w = scale0 * a.w + scale1 * b.w;
		retVal.Normalize();
		return retVal;
	}

	// Concatenate
	// Rotate by q FOLLOWED BY p
	static FTQuaternion Concatenate(const FTQuaternion& q, const FTQuaternion& p)
	{
		FTQuaternion retVal;

		// Vector component is:
		// ps * qv + qs * pv + pv x qv
		FTVector3 qv(q.x, q.y, q.z);
		FTVector3 pv(p.x, p.y, p.z);
		FTVector3 newVec = p.w * qv + q.w * pv + FTVector3::Cross(pv, qv);
		retVal.x = newVec.x;
		retVal.y = newVec.y;
		retVal.z = newVec.z;

		// Scalar component is:
		// ps * qs - pv . qv
		retVal.w = p.w * q.w - FTVector3::Dot(pv, qv);

		return retVal;
	}

	static const FTQuaternion Identity;
};

namespace Color
{
	static const FTVector3 Black(0.0f, 0.0f, 0.0f);
	static const FTVector3 White(1.0f, 1.0f, 1.0f);
	static const FTVector3 Red(1.0f, 0.0f, 0.0f);
	static const FTVector3 Green(0.0f, 1.0f, 0.0f);
	static const FTVector3 Blue(0.0f, 0.0f, 1.0f);
	static const FTVector3 Yellow(1.0f, 1.0f, 0.0f);
	static const FTVector3 LightYellow(1.0f, 1.0f, 0.88f);
	static const FTVector3 LightBlue(0.68f, 0.85f, 0.9f);
	static const FTVector3 LightPink(1.0f, 0.71f, 0.76f);
	static const FTVector3 LightGreen(0.56f, 0.93f, 0.56f);
}
