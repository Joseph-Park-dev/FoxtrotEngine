#pragma once
#include "FTMath.h"

namespace D3D11
{
	struct TextAttribute
	{
		float			Rotation;
		Math::FTVector2 Offset;
		Math::FTVector2 Origin;
		Math::FTVector2 Scale;
		Math::FTVector4 Color;

		/// @brief Initializes text content and its placement attributes.
		/// @note Initializes the Rotation base or delegates to its constructor.
		TextAttribute()
			: Rotation(0.0f)
			, Offset(Math::FTVector2(0.0f, 0.0f))
			, Origin(Math::FTVector2(0.0f, 0.0f))
			, Scale(Math::FTVector2(0.0f, 0.0f))
			, Color(Math::FTVector4(0.f))
		{
		}

		/// @brief Copies the supplied value into this object.
		/// @param origin Source instance whose state is copied.
		/// @return Value produced by the overloaded operation.
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
