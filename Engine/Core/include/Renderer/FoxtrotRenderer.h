// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Main renderer from FoxtrotEngine.
/// Takes care of the rendering operations in DirectX 11 context.
/// </summary>

#pragma once
#include "Entity/Entity.h"

namespace Math
{
	class FTVector2;
}

namespace Core
{
	class FTWindow;
	class FoxtrotRenderer
	{
	protected:
		virtual bool Initialize(FTWindow* window) = 0;
	};
} // namespace Core