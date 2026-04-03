#pragma once
#include <Math/FTMath.h>

namespace D3D11
{
	struct TextAttribute
	{
		float	  Rotation;
		FTVector2 Offset;
		FTVector2 Origin;
		FTVector2 Scale;
		FTVector4 Color;

		TextAttribute()
			: Rotation(0.0f)
			, Offset(FTVector2(0.0f, 0.0f))
			, Origin(FTVector2(0.0f, 0.0f))
			, Scale(FTVector2(0.0f, 0.0f))
			, Color(FTVector4(0.f))
		{
		}

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
} // namespace D3D11