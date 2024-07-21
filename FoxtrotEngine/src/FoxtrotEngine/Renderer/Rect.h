#pragma once
#include "FoxtrotEngine/Math/FTMath.h"

class Rect
{
public:
	bool Contains(const FTVector2& point);
	bool Overlaps(const Rect& other);

public:
	FTVector2 GetSize() { return mSize; }

	void Set(float x, float y, float width, float height) {
		mCenter		= FTVector2(x, y);
		mWidth		= width;
		mHeight		= height;
		mMax		= mCenter + FTVector2(width, height) / 2;
		mMin		= mCenter - FTVector2(width, height) / 2;
		mPosition	= mMin;
		mSize		= FTVector2(width, height);
	}
	static const Rect Zero;

public:
	bool operator==(Rect& other) {
		return this->mMin == other.mMin 
			&& this->mMax == other.mMax;
	}
public:
	Rect(float x, float y, float width, float height) {
		Set(x, y, width, height);
	}

private:
	FTVector2	mCenter;
	float		mWidth;
	float		mHeight;
	FTVector2	mMax;
	FTVector2	mMin;
	FTVector2	mPosition;
	FTVector2	mSize;
};

const Rect Rect::Zero(0.0f, 0.0f, 0.0f, 0.0f);