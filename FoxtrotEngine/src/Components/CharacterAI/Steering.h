// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// A class based on the "Steering Behaviors For Autonomous Characters" by Craig W. Reynolds.
/// https://www.red3d.com/cwr/steer/gdc99/
/// This sets the object's movement scheme.
/// </summary>

#pragma once
#include "Math/FTMath.h"

struct Steering
{
	float Angular;
	FTVector3 Linear;

	static Steering Halt()
	{
		return Steering(0.f, FTVector3::Zero);
	}

	Steering(float angular, FTVector3 linear)
		: Angular(angular)
		, Linear (linear)
	{}
};