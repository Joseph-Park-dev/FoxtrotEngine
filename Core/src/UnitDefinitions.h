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

float Meters(float value) { return value * UNITFIGURE; }
float Centimeters(float value) { return Meters(value) / 100.f; }