// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Class that defines rectangle area in the 2D game world.
/// Mostly used for rendering purposes
/// </summary>

#pragma once
#include "Math/FTMath.h"

class FTRect
{
public:
	bool Overlaps(const FTVector2& point);
	bool Overlaps(const FTRect& other);

public:
	const FTVector2& GetSize() { return mSize; }
	const FTVector2& GetCenter() { return mCenter; }
	const FTVector2& GetMin() { return mMin; }
	const FTVector2& GetMax() { return mMax; }

	void SetPosition(FTVector2 val);
	void SetSize(FTVector2 val);

	// Update FTRect values using the existing member variables
	void Set();
	// Update FTRect values using the new ones.
	void Set(FTVector2 center, FTVector2 dimension);
	void Set(float posX, float posY, float width, float height);

	static const FTRect Zero;

public:
	bool operator==(FTRect& other)
	{
		return this->mMin == other.mMin && this->mMax == other.mMax;
	}

public:
	FTRect()
	{
		Set(0.f, 0.f, 0.f, 0.f);
	}

	FTRect(float x, float y, float width, float height)
	{
		Set(x, y, width, height);
	}
	void CloneTo(FTRect* rect);

private:
	FTVector2 mCenter;
	float	  mWidth;
	float	  mHeight;
	FTVector2 mMax;
	FTVector2 mMin;
	FTVector2 mSize;

public:
	void SaveProperties(std::ofstream& ifs);
	void LoadProperties(std::ifstream& ofs);

#ifdef FOXTROT_EDITOR
public:
	void UpdateUI();
#endif
};

namespace ChunkKey
{
	constexpr const char* FTRECT		= "FTRect";
	constexpr const char* FTRECT_CENTER = "Center";
	constexpr const char* FTRECT_SIZE	= "Size";
} // namespace ChunkKey