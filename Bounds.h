#pragma once
#include "Math.h"
struct Bounds
{
	float x, y, w, h;

	Vector2 GetCenter()	 const { return Vector2(x, y); }
	Vector2 GetSize()	 const { return Vector2(w, h); }
	Vector2 GetExtents() const { return GetSize() * 0.5f; }
	Vector2 GetMax()	 const { return GetCenter() + GetExtents(); }
	Vector2 GetMin()	 const { return GetCenter() - GetExtents(); }

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
	Bounds(Vector2 center, Vector2 size)
		: x(center.x), y(center.y)
		, w(size.x), h(size.y)
	{}
};

