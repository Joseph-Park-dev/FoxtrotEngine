#pragma once
#include "Math/FTMath.h"

class FTRect
{
public:
	bool Contains(const FTVector2& point);
	bool Overlaps(const FTRect& other);

public:
	const FTVector2& GetSize	 () { return mSize; }
	const FTVector2& GetPosition() { return mPosition; }
	const FTVector2& GetMin() { return mMin; }
	const FTVector2& GetMax () { return mMax; }

	void Set(float posX, float posY, float width, float height) {
		mWidth		= width;
		mHeight		= height;
		mCenter		= FTVector2(posX, posY) + FTVector2(width, height) / 2;
		mMin		= FTVector2(posX, posY);
		mMax		= mMin + FTVector2(width, height);
		mPosition	= mMin;
		mSize		= FTVector2(width, height);
	}
	static const FTRect Zero;

public:
	bool operator==(FTRect& other) {
		return this->mMin == other.mMin 
			&& this->mMax == other.mMax;
	}
public:
	FTRect(float x, float y, float width, float height) {
		Set(x, y, width, height);
	}

private:
	FTVector2	mCenter;
	float		mWidth;
	float		mHeight;
	FTVector2	mMax;
	FTVector2	mMin;
	FTVector2	mPosition;  // Left Top position of the tile.
	FTVector2	mSize;
};