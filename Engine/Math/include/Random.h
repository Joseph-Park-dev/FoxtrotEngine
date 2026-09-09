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

#pragma once
#include <random>

#include "FTMath.h"

namespace Math
{
	class FTVector2;
	class FTVector3;

	class Random
	{
	public:
		/// @brief Seeds the shared pseudo-random generator from std::random_device.
		/// @note Changes the sequence used by all Random methods. Synchronize concurrent access externally.
		/// @throws std::exception If the system random source cannot be initialized or read.
		/// @note Advances or reseeds shared generator state; synchronize concurrent access externally.
		static void Init();

		/// @brief Restarts the shared pseudo-random sequence with a reproducible seed.
		/// @param seed Initial state used to reproduce a sequence of random samples.
		/// @note Synchronize concurrent access to the shared generator externally.
		/// @note Advances or reseeds shared generator state; synchronize concurrent access externally.
		static void Seed(unsigned int seed);

		// Get a float between 0.0f and 1.0f
		/// @brief Draws a uniform pseudo-random sample in [0, 1).
		/// @return Sample in [0, 1).
		/// @note Advances or reseeds shared generator state; synchronize concurrent access externally.
		static float GetFloat();

		// Get a float from the specified range
		/// @brief Draws a uniform pseudo-random sample in [min, max).
		/// @param min Lower bound.
		/// @param max Upper bound.
		/// @return Sample in [min, max).
		/// @pre min must not exceed max.
		/// @note Advances or reseeds shared generator state; synchronize concurrent access externally.
		static float GetFloatRange(float min, float max);

		// Get an int from the specified range
		/// @brief Draws a uniform pseudo-random integer in the inclusive range [min, max].
		/// @param min Lower bound.
		/// @param max Upper bound.
		/// @return Integer sample in [min, max].
		/// @pre min must not exceed max.
		/// @note Advances or reseeds shared generator state; synchronize concurrent access externally.
		static int GetIntRange(int min, int max);

		// Get a random vector given the min/max bounds
		/// @brief Draws independent uniform samples and interpolates each component between its bounds.
		/// @param min Lower bound.
		/// @param max Upper bound.
		/// @return Vector with independently sampled components.
		/// @note Advances or reseeds shared generator state; synchronize concurrent access externally.
		static FTVector2 GetVector(const FTVector2& min, const FTVector2& max);
		/// @brief Draws independent uniform samples and interpolates each component between its bounds.
		/// @param min Lower bound.
		/// @param max Upper bound.
		/// @return Vector with independently sampled components.
		/// @note Advances or reseeds shared generator state; synchronize concurrent access externally.
		static FTVector3 GetVector(const FTVector3& min, const FTVector3& max);

	private:
		static std::mt19937 sGenerator;
	};
} // namespace Core
