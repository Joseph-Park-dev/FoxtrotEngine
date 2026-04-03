#pragma once
#include <Math/FTMath.h>

namespace D3D11
{
	struct TextVertex
	{
		TextVertex() {}

		TextVertex(float r, float g, float b, float a, float u, float v, float tw, float th, float x, float y, float w, float h)
			: color(r, g, b, a), texCoord(u, v, tw, th), pos(x, y, w, h) {}

		FTVector4 pos;
		FTVector4 color;
		FTVector4 texCoord;
	};
} // namespace D3D11