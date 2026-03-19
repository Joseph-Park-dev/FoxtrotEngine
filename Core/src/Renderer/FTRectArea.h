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
#include <iosfwd>

#include "Math/FTMath.h"

class FTRectArea
{
public:
	bool Overlaps(const FTVector2& point);
	// bool Overlaps(const FTRectArea& other);

public:
	FTVector2&		 GetSize() { return mSize; }
	const FTVector2& GetCenter() { return mCenter; }
	const FTVector2& GetMin() { return mMin; }
	const FTVector2& GetMax() { return mMax; }
	const float&	 GetRotAngle() { return mRotAngle; }

	// Update FTRectArea values using the new ones.
	void Set(FTVector2 center, FTVector2 dimension, float rotAngle = 0);

	/// @brief Update FTRectArea values
	/// @param posX Top left position X
	/// @param posY Top left position Y
	void Set(float posX, float posY, float width, float height, float rotAngle = 0);

	static const FTRectArea Zero;

public:
	bool operator==(FTRectArea& other)
	{
		return this->mMin == other.mMin && this->mMax == other.mMax;
	}

public:
	FTRectArea();
	FTRectArea(float x, float y, float width, float height, float rotAngle = 0);
	void CloneTo(FTRectArea* rect);

private:
	FTVector2 mCenter;
	float	  mWidth;
	float	  mHeight;
	float	  mRotAngle; // Rotated angle in radian
	FTVector2 mSize;
	FTVector2 mMax;
	FTVector2 mMin;

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
	constexpr const char* FTRectArea		  = "FTRectArea";
	constexpr const char* FTRectArea_CENTER	  = "Center";
	constexpr const char* FTRectArea_SIZE	  = "Size";
	constexpr const char* FTRectArea_ROTANGLE = "RotAngle";
} // namespace ChunkKey