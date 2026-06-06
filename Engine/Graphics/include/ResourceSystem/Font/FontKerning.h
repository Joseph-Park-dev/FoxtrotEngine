// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
namespace Graphics
{
	struct FontKerning
	{
		int	  firstid;	// the first character
		int	  secondid; // the second character
		float amount;	// the amount to add/subtract to second characters x
	};
} // namespace Graphics