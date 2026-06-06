#pragma once
#include "FTMath.h"

namespace Graphics
{
	struct TextVertex
	{
		TextVertex() {}

		TextVertex(float r, float g, float b, float a, float u, float v, float tw, float th, float x, float y, float w, float h)
			: Color(r, g, b, a), TexCoord(u, v, tw, th), Pos(x, y, w, h) {}

		Math::FTVector4 Pos		 = Math::FTVector4(0.f);
		Math::FTVector4 Color	 = Math::FTVector4(0.f);
		Math::FTVector4 TexCoord = Math::FTVector4(0.f);
	};
} // namespace Graphics