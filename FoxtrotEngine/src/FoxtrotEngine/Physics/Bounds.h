#pragma once
#include "FoxtrotEngine/Math/FTMath.h"

struct Bounds
{
	float x, y, w, h;

	FTVector2 GetCenter()	 const { return FTVector2(x, y); }
	FTVector2 GetSize()	 const { return FTVector2(w, h); }
	FTVector2 GetExtents() const { return GetSize() * 0.5f; }
	FTVector2 GetMax()	 const { return GetCenter() + GetExtents(); }
	FTVector2 GetMin()	 const { return GetCenter() - GetExtents(); }

	Bounds()
		: x(0.f), y(0.f), w(0.f), h(0.f)
	{}
	Bounds(int pX, int pY, int pW, int pH)
		: x(static_cast<float>(pX))
		, y(static_cast<float>(pY))
		, w(static_cast<float>(pW))
		, h(static_cast<float>(pH))
	{}
	Bounds(float pX, float pY, float pW, float pH)
		: x(pX), y(pY), w(pW), h(pH)
	{}
	Bounds(FTVector2 center, FTVector2 size)
		: x(center.x), y(center.y)
		, w(size.x), h(size.y)
	{}
};

