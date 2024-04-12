#pragma once
#include <SDL2/SDL.h>
#include "Math.h"
class Bounds
{
public:
	Vector2 GetCenter()	 const { return mCenter; }
	Vector2 GetSize()	 const { return mSize; }
	Vector2 GetExtents() const { return GetSize() * 0.5f; }
	Vector2 GetMax()	 const { return mCenter + GetExtents(); }
	Vector2 GetMin()	 const { return mCenter - GetExtents(); }

	void	SetCenter(Vector2 val) { mCenter = val; }
	void	SetSize	 (Vector2 val) { mSize = val; }

public:

	Bounds(int x, int y, int w, int h)
		: mCenter(Vector2(static_cast<float>(x), static_cast<float>(y)))
		, mSize(Vector2(static_cast<float>(w), static_cast<float>(h)))
	{}
	Bounds(float x, float y, float w, float h)
		: mCenter(Vector2(x,y))
		, mSize(Vector2(w, h))
	{}
	Bounds(Vector2 center, Vector2 size)
		: mCenter(center)
		, mSize(size)
	{}
	Bounds(SDL_Rect* rect)
		: mCenter(Vector2(rect->x, rect->y))
		, mSize(Vector2(rect->w, rect->h))
	{}

private:
	Vector2 mCenter;
	Vector2 mSize;
};

