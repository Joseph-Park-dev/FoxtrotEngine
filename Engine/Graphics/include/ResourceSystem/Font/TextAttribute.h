// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "FTMath.h"

namespace Graphics
{
	struct TextAttribute
	{
		float			Rotation = 0.0f;
		Math::FTVector2 Offset	 = Math::FTVector2::Zero;
		Math::FTVector2 Origin	 = Math::FTVector2::Zero;
		Math::FTVector2 Scale	 = Math::FTVector2::Zero;
		Math::FTVector4 Color	 = Math::FTVector4(0.f);

		TextAttribute* operator=(TextAttribute* origin)
		{
			this->Rotation = origin->Rotation;
			this->Offset   = origin->Offset;
			this->Color	   = origin->Color;
			this->Origin   = origin->Origin;
			this->Scale	   = origin->Scale;
			return this;
		}
	};
} // namespace Graphics