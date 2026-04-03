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
#include "Math/FTMath.h"

namespace D3D11
{
	struct Vertex
	{
		Vertex() {}

		Vertex(float x, float y)
		{
			position.x = x;
			position.y = y;
		}

#ifdef FOXTROT_EDITOR
		Vertex(b2Vec2 vec2)
		{
			position.x = vec2.x;
			position.y = vec2.y;
		}
#endif // FOXTROT_EDITOR

		FTVector3 position;
		FTVector4 color;
		FTVector2 texcoord;

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
		FTVector3 Position;
	};

	struct DebugVertex
	{
		DebugVertex() {}

		DebugVertex(float x, float y)
		{
			position.x = x;
			position.y = y;
		}

#ifdef FOXTROT_EDITOR
		DebugVertex(b2Vec2 vec2)
		{
			position.x = vec2.x;
			position.y = vec2.y;
		}
#endif // FOXTROT_EDITOR

		FTVector3 position;
		FTVector4 color;
	};
} // namespace D3D11