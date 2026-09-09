// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Intended to be used for box2D rigidbody simulation.
/// These may not be used and if so, these will be removed.
/// </summary>

#pragma once
#define UNITFIGURE 10.f

namespace Core
{
	/// @brief Converts a distance to the engine's meter-based unit convention.
	/// @param value Value to assign, insert, or process.
	/// @return Converts a distance to the engine's meter-based unit convention.
	float Meters(float value) { return value * UNITFIGURE; }
	/// @brief Converts a centimeter distance to the engine's meter-based unit convention.
	/// @param value Value to assign, insert, or process.
	/// @return Converts a centimeter distance to the engine's meter-based unit convention.
	float Centimeters(float value) { return Meters(value) / 100.f; }
} // namespace Core
