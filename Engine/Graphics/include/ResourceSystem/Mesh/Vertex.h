// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Vertex Data in DirectX 11 context.
/// </summary>

#pragma once
#include "FTMath.h"

namespace Graphics
{
	struct Vertex
	{
		Vertex() {}

		Vertex(float x, float y)
		{
			position.x = x;
			position.y = y;
		}

		Math::FTVector3 position;
		Math::FTVector4 color;
		Math::FTVector2 texcoord;

		Vertex operator=(Vertex& v)
		{
			position = v.position;
			color	 = v.color;
			texcoord = v.texcoord;

			return *this;
		}
	};

	struct SpriteVertex
	{
		Math::FTVector3 Position;
	};

	struct DebugVertex
	{
		DebugVertex() {}

		DebugVertex(float x, float y)
		{
			position.x = x;
			position.y = y;
		}

		Math::FTVector3 position;
		Math::FTVector4 color;
	};
} // namespace Graphics