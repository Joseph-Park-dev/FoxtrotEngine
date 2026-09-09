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
#include "Random.h"

namespace Math
{
	/// @brief Seeds the shared pseudo-random generator from the system random source.
	/// @note Advances or reseeds shared generator state; synchronize concurrent access externally.
	/// @throws std::exception If the system random source cannot be initialized or read.
	void Random::Init()
	{
		std::random_device rd;
		Random::Seed(rd());
	}

	/// @brief Restarts the shared pseudo-random sequence with a reproducible seed.
	/// @param seed Seed used to reproduce the sequence of subsequent samples.
	/// @note Advances or reseeds shared generator state; synchronize concurrent access externally.
	void Random::Seed(unsigned int seed)
	{
		sGenerator.seed(seed);
	}

	/// @brief Draws a uniform pseudo-random sample in [0, 1).
	/// @return Sample in [0, 1).
	/// @note Advances or reseeds shared generator state; synchronize concurrent access externally.
	float Random::GetFloat()
	{
		return GetFloatRange(0.0f, 1.0f);
	}

	/// @brief Draws a uniform pseudo-random sample in [min, max).
	/// @param min Lower bound.
	/// @param max Upper bound.
	/// @return Sample in [min, max).
	/// @pre min must not exceed max.
	/// @note Advances or reseeds shared generator state; synchronize concurrent access externally.
	float Random::GetFloatRange(float min, float max)
	{
		std::uniform_real_distribution<float> dist(min, max);
		return dist(sGenerator);
	}

	/// @brief Draws a uniform pseudo-random integer in the inclusive range [min, max].
	/// @param min Lower bound.
	/// @param max Upper bound.
	/// @return Integer sample in [min, max].
	/// @pre min must not exceed max.
	/// @note Advances or reseeds shared generator state; synchronize concurrent access externally.
	int Random::GetIntRange(int min, int max)
	{
		std::uniform_int_distribution<int> dist(min, max);
		return dist(sGenerator);
	}

	/// @brief Draws independent uniform samples and interpolates each component between its bounds.
	/// @param min Lower bound.
	/// @param max Upper bound.
	/// @return Vector with independently sampled components.
	/// @note Advances or reseeds shared generator state; synchronize concurrent access externally.
	FTVector2 Random::GetVector(const FTVector2& min, const FTVector2& max)
	{
		FTVector2 r = FTVector2(GetFloat(), GetFloat());
		return min + (max - min) * r;
	}

	/// @brief Draws independent uniform samples and interpolates each component between its bounds.
	/// @param min Lower bound.
	/// @param max Upper bound.
	/// @return Vector with independently sampled components.
	/// @note Advances or reseeds shared generator state; synchronize concurrent access externally.
	FTVector3 Random::GetVector(const FTVector3& min, const FTVector3& max)
	{
		FTVector3 r = FTVector3(GetFloat(), GetFloat(), GetFloat());
		return min + (max - min) * r;
	}

	std::mt19937 Random::sGenerator;
} // namespace Core
