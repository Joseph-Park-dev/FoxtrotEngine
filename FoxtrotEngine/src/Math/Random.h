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
/// This is used to create random values.
/// </summary>

#pragma  once
#include <random>

#include "Math/FTMath.h"

class Random
{
public:
	static void Init();

	// Seed the generator with the specified int
	// NOTE: You should generally not need to manually use this
	static void Seed(unsigned int seed);

	// Get a float between 0.0f and 1.0f
	static float GetFloat();
	
	// Get a float from the specified range
	static float GetFloatRange(float min, float max);

	// Get an int from the specified range
	static int GetIntRange(int min, int max);

	// Get a random vector given the min/max bounds
	static FTVector2 GetVector(const FTVector2& min, const FTVector2& max);
	static FTVector3 GetVector(const FTVector3& min, const FTVector3& max);
private:
	static std::mt19937 sGenerator;
};
