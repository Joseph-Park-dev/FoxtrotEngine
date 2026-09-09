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
		/// @brief Tests whether two rectangular areas intersect.
		/// @param point Point to test or draw.
		/// @return True when two rectangular areas intersect; otherwise false.
		bool Overlaps(const Math::FTVector2& point);
		// bool Overlaps(const FTRectArea& other);

	public:
		/// @brief Returns the size used by this ftrect area.
		/// @return Borrowed access to the size.
		const Math::FTVector2& GetSize() { return mSize; }
		/// @brief Returns the center used by this ftrect area.
		/// @return Borrowed access to the center.
		const Math::FTVector2& GetCenter() { return mCenter; }
		/// @brief Returns the min used by this ftrect area.
		/// @return Borrowed access to the min.
		const Math::FTVector2& GetMin() { return mMin; }
		/// @brief Returns the max used by this ftrect area.
		/// @return Borrowed access to the max.
		const Math::FTVector2& GetMax() { return mMax; }
		/// @brief Returns the rot angle used by this ftrect area.
		/// @return Borrowed access to the rot angle.
		const float&		   GetRotAngle() { return mRotAngle; }

		// Update FTRectArea values using the new ones.
		/// @brief Updates the  used by subsequent operations.
		/// @param center Center position of the geometry.
		/// @param dimension Dimensions used when creating the resource.
		/// @param rotAngle Rotation angle.
		void Set(Math::FTVector2 center, Math::FTVector2 dimension, float rotAngle = 0);

		/// @brief Update FTRectArea values
		/// @param posX Top left position X
		/// @param posY Top left position Y
		/// @param width Width of the window, texture, or geometry.
		/// @param height Height of the window, texture, or geometry.
		/// @param rotAngle Rotation angle.
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
		/// @brief Compares the stored value or iterator position with the operand.
		/// @param other Source object or comparison operand.
		/// @return True when the compared values match; otherwise false.
		bool operator==(FTRectArea& other)
		{
			return this->mMin == other.mMin && this->mMax == other.mMax;
		}

	public:
		/// @brief Initializes the origin and dimensions of a rendering rectangle.
		FTRectArea();
		/// @brief Initializes the origin and dimensions of a rendering rectangle.
		/// @param x Horizontal coordinate or X component.
		/// @param y Vertical coordinate or Y component.
		/// @param width Width of the window, texture, or geometry.
		/// @param height Height of the window, texture, or geometry.
		/// @param rotAngle Rotation angle.
		FTRectArea(float x, float y, float width, float height, float rotAngle = 0)
		{
			Set(x, y, width, height, rotAngle);
		}
		/// @brief Copies this object's state into the requested target or attaches its clone to the target actor.
		/// @param rect Rectangle bounds used by the operation.
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
		/// @brief Builds the editor controls for inspecting and modifying this object's state.
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
