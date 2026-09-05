// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Renderer/FTRectArea.h"

#include "FTMath.h"
#include "FileSystem/FileIOHelper.h"
#include "FTDS/Static/FTString.h"

#ifdef FOXTROT_EDITOR
	#include "CommandHistory.h"
	#include "Utility/EditorHelper.h"
#endif

namespace D3D11
{
	const FTRectArea FTRectArea::Zero(0.0f, 0.0f, 0.0f, 0.0f);

	bool FTRectArea::Overlaps(const Math::FTVector2& point)
	{
		Math::FTVector2 v1 = Math::FTVector2(Math::Cos(mRotAngle), Math::Sin(mRotAngle));
		Math::FTVector2 v2 = Math::FTVector2(-v1.y, v1.x); // Rotate by 90

		// scale them appropriately by the dimensions
		v1 *= mWidth / 2;
		v2 *= mHeight / 2;

		Math::FTVector2 p0 = mCenter + v1 + v2;
		Math::FTVector2 p1 = mCenter - v1 + v2;
		Math::FTVector2 p2 = mCenter - v1 - v2;
		Math::FTVector2 p3 = mCenter + v1 - v2;

		return Math::PointInRectangle(point, p0, p1, p2, p3);
	}

	// bool FTRectArea::Overlaps(const FTRectArea& other)
	//{
	//	Math::FTVector2 rightMin = other.mMin;
	//	Math::FTVector2 rightMax = other.mMax;
	//	return Overlaps(other.mMin) || Overlaps(other.mMax);
	// }

	void FTRectArea::Set(Math::FTVector2 center, Math::FTVector2 dimension, float rotAngle)
	{
		mCenter	  = center;
		mSize	  = dimension;
		mWidth	  = dimension.x;
		mHeight	  = dimension.y;
		mMin	  = mCenter - mSize / 2;
		mMax	  = mCenter + mSize / 2;
		mRotAngle = rotAngle;
	}

	FTRectArea::FTRectArea()
	{
		Set(0.f, 0.f, 0.f, 0.f);
	}

	void FTRectArea::CloneTo(FTRectArea* rect)
	{
		rect->mCenter = mCenter;
		rect->mWidth  = mWidth;
		rect->mHeight = mHeight;
		rect->mSize	  = mSize;
		rect->mMin	  = mMin;
		rect->mMax	  = mMax;
	}

#ifdef FOXTROT_EDITOR
	void FTRectArea::UpdateUI()
	{
		Editor::UPDATE_VEC2("Center", mCenter);
		Editor::UPDATE_VEC2("Size", mSize);
		Editor::UPDATE_FLOAT("Rot Angle", mRotAngle);
		Set(mCenter, mSize, mRotAngle);
	}
#endif

	FTRectArea* CreateFTRectArea()
	{
		return DBG_NEW FTRectArea();
	}

	FTRectArea* CreateFTRectArea(float x, float y, float width, float height, float rotAngle)
	{
		return DBG_NEW FTRectArea(x, y, width, height, rotAngle);
	}

	void SaveProperties(std::ofstream& ofs, FTRectArea* rectArea)
	{
		Common::FileIOHelper::BeginDataPackSave(ofs, ChunkKey::FTRectArea);
		Common::FileIOHelper::SaveVector2(ofs, ChunkKey::FTRectArea_CENTER, rectArea->GetCenter());
		Common::FileIOHelper::SaveVector2(ofs, ChunkKey::FTRectArea_SIZE, rectArea->GetSize());
		Common::FileIOHelper::SaveFloat(ofs, ChunkKey::FTRectArea_ROTANGLE, rectArea->GetRotAngle());
		Common::FileIOHelper::EndDataPackSave(ofs, ChunkKey::FTRectArea);
	}

	void LoadProperties(std::ifstream& ifs, FTRectArea* rectArea)
	{
		float			rotAngle = 0.f;
		Math::FTVector2 size(0.f);
		Math::FTVector2 center(0.f);

		Common::FileIOHelper::BeginDataPackLoad(ifs);
		Common::FileIOHelper::LoadFloat(ifs, rotAngle);
		Common::FileIOHelper::LoadVector2(ifs, size);
		Common::FileIOHelper::LoadVector2(ifs, center);
		rectArea->Set(center, size, rotAngle);
	}
} // namespace D3D11