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
#include "FTMath.h"

namespace D3D11
{
	class FTRectArea
	{
	public:
		bool Overlaps(const Math::FTVector2& point);
		// bool Overlaps(const FTRectArea& other);

	public:
		const Math::FTVector2& GetSize() { return mSize; }
		const Math::FTVector2& GetCenter() { return mCenter; }
		const Math::FTVector2& GetMin() { return mMin; }
		const Math::FTVector2& GetMax() { return mMax; }
		const float&		   GetRotAngle() { return mRotAngle; }

		// Update FTRectArea values using the new ones.
		void Set(Math::FTVector2 center, Math::FTVector2 dimension, float rotAngle = 0);

		/// @brief Update FTRectArea values
		/// @param posX Top left position X
		/// @param posY Top left position Y
		void Set(float posX, float posY, float width, float height, float rotAngle = 0)
		{
			mWidth	  = width;
			mHeight	  = height;
			mSize	  = Math::FTVector2(width, height);
			mCenter	  = Math::FTVector2(posX, posY) + (mSize * 0.5f);
			mMin	  = mCenter - mSize / 2;
			mMax	  = mCenter + mSize / 2;
			mRotAngle = rotAngle;
		}

		static const FTRectArea Zero;

	public:
		bool operator==(FTRectArea& other)
		{
			return this->mMin == other.mMin && this->mMax == other.mMax;
		}

	public:
		FTRectArea();
		FTRectArea(float x, float y, float width, float height, float rotAngle = 0)
		{
			Set(x, y, width, height, rotAngle);
		}
		void CloneTo(FTRectArea* rect);

	private:
		Math::FTVector2 mCenter;
		float			mWidth;
		float			mHeight;
		float			mRotAngle; // Rotated angle in radian
		Math::FTVector2 mSize;
		Math::FTVector2 mMax;
		Math::FTVector2 mMin;

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
} // namespace D3D11