#pragma once
#include "FTMath.h"

namespace Graphics
{
	struct TextVertex
	{
		/// @brief Initializes glyph vertex position and texture coordinates.
		TextVertex() {}

		/// @brief Initializes glyph vertex position and texture coordinates.
		/// @param r Red color component.
		/// @param g Green color component.
		/// @param b Second operand.
		/// @param a First operand.
		/// @param u Horizontal texture coordinate.
		/// @param v Vertical texture coordinate.
		/// @param tw Texture width.
		/// @param th Texture height.
		/// @param x Horizontal coordinate or X component.
		/// @param y Vertical coordinate or Y component.
		/// @param w W component.
		/// @param h Height.
		/// @note Initializes the Color base or delegates to its constructor.
		TextVertex(float r, float g, float b, float a, float u, float v, float tw, float th, float x, float y, float w, float h)
			: Color(r, g, b, a), TexCoord(u, v, tw, th), Pos(x, y, w, h) {}

		Math::FTVector4 Pos		 = Math::FTVector4(0.f);
		Math::FTVector4 Color	 = Math::FTVector4(0.f);
		Math::FTVector4 TexCoord = Math::FTVector4(0.f);
	};
} // namespace Graphics
