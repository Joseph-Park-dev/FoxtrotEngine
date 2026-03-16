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

FTVector4::FTVector4(float in)
{
	x = in;
	y = in;
	z = in;
	w = in;
}

FTVector4::FTVector4(float inX, float inY, float inZ, float inA)
{
	x = inX;
	y = inY;
	z = inZ;
	w = inA;
}