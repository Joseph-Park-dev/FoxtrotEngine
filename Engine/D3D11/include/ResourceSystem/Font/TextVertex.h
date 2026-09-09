#pragma once
#include "FTMath.h"

namespace D3D11
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
		/// @note Initializes the color base or delegates to its constructor.
		TextVertex(float r, float g, float b, float a, float u, float v, float tw, float th, float x, float y, float w, float h)
			: color(r, g, b, a), texCoord(u, v, tw, th), pos(x, y, w, h) {}

		Math::FTVector4 pos;
		Math::FTVector4 color;
		Math::FTVector4 texCoord;
	};
} // namespace D3D11
