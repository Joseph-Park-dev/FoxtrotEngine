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
		/// @brief Initializes mesh vertex attributes.
		Vertex() {}

		/// @brief Initializes mesh vertex attributes.
		/// @param x Horizontal coordinate or X component.
		/// @param y Vertical coordinate or Y component.
		Vertex(float x, float y)
		{
			position.x = x;
			position.y = y;
		}

		Math::FTVector3 position;
		Math::FTVector4 color;
		Math::FTVector2 texcoord;

		/// @brief Copies the supplied value into this object.
		/// @param v Vertical texture coordinate.
		/// @return Value produced by the overloaded operation.
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
		/// @brief Initializes a vertex used for debug geometry.
		DebugVertex() {}

		/// @brief Initializes a vertex used for debug geometry.
		/// @param x Horizontal coordinate or X component.
		/// @param y Vertical coordinate or Y component.
		DebugVertex(float x, float y)
		{
			position.x = x;
			position.y = y;
		}

		Math::FTVector3 position;
		Math::FTVector4 color;
	};
} // namespace Graphics
