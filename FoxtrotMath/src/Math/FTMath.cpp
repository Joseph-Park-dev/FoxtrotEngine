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
#include "pch.h"
#include "FTMath.h"

using Matrix = DirectX::SimpleMath::Matrix;
using Vector2 = DirectX::SimpleMath::Vector2;
using Vector3 = DirectX::SimpleMath::Vector3;

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

FTVector3::FTVector3(FTVector2 vec2)
	: x(vec2.x)
	, y(vec2.y)
	, z(0.f)
{
}

FTVector3::FTVector3(b2Vec2 vec2)
	: x(vec2.x)
	, y(vec2.y)
	, z(0.0f)
{
}

FTVector3::FTVector3(DirectX::SimpleMath::Vector3 vec3)
	: x(vec3.x)
	, y(vec3.y)
	, z(vec3.z)
{
}

void FTVector3::DecomposeMatrix(
	FTVector3& scale, FTVector3& rot, FTVector3& pos, DirectX::SimpleMath::Matrix& matrix)
{
	DirectX::SimpleMath::Quaternion quat;
	DirectX::SimpleMath::Vector3 dxScale, dxPosition;
	matrix.Decompose(dxScale, quat, dxPosition);

	scale = dxScale;
	rot = Math::QuaternionToEuler(quat);
	pos = dxPosition;
}

const b2Vec2 FTVector3::GetB2Vec2() const
{
	return b2Vec2{ this->x, this->y };
}

const DirectX::XMFLOAT3 FTVector3::GetDXVec3() const
{
	return DirectX::XMFLOAT3(this->x, this->y, this->z);
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

FTVector2 FTVector2::CubicBezierVelocity2D(const FTVector2& P0, const FTVector2& P1, const FTVector2& P2, const FTVector2& P3, double t)
{
	double u = 1.0 - t;
	double u2 = u * u;
	double t2 = t * t;

	FTVector2 velocity;
	velocity.x = 3 * u2 * (P1.x - P0.x) + 6 * u * t * (P2.x - P1.x) + 3 * t2 * (P3.x - P2.x);
	velocity.y = 3 * u2 * (P1.y - P0.y) + 6 * u * t * (P2.y - P1.y) + 3 * t2 * (P3.y - P2.y);
	return velocity;
}

FTVector2 FTVector2::PlotCircularFall(float angle, float dist)
{
	return FTVector2(dist * Math::Cos(angle), dist * Math::Sin(angle));
}
