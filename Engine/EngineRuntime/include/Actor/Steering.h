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
#include "Debugging/DebugMemAlloc.h"
#include "FTMath.h"

namespace Core
{
	class Math::FTVector3;

	struct Steering
	{
		float	  Angular;
		Math::FTVector3 Linear; // Normalized direction of movement

		bool JumpTriggered;

		/// @brief Allocates steering state with no requested motion.
		/// @return Created empty steering instance or resource.
		static Steering* CreateEmptySteering()
		{
			return DBG_NEW Steering(0.f, Math::FTVector3::Zero);
		}

		/// @brief Produces steering state with no requested angular or linear motion.
		/// @return Steering state with no requested angular or linear motion.
		static Steering Halt()
		{
			return Steering(0.f, Math::FTVector3::Zero);
		}

		/// @brief Initializes requested angular and linear motion with jumping disabled.
		/// @param angular Requested angular motion.
		/// @param linear Requested linear motion.
		/// @note Initializes the Angular base or delegates to its constructor.
		Steering(float angular, Math::FTVector3 linear)
			: Angular(angular)
			, Linear(linear)
			, JumpTriggered(false)
		{
		}

		friend bool operator==(Steering left, Steering right)
		{
			return left.Linear.x == right.Linear.x && left.Linear.y == right.Linear.y && left.Angular == right.Angular && left.JumpTriggered == right.JumpTriggered;
		}

		friend bool operator!=(Steering left, Steering right)
		{
			return left.Linear.x != right.Linear.x || left.Linear.y != right.Linear.y || left.Angular != right.Angular || left.JumpTriggered != right.JumpTriggered;
		}
	};
} // namespace Core
