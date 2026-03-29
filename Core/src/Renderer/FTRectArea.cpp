// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Renderer/FTRectArea.h"

#include "Math/FTMath.h"
#include "FileSystem/FileIOHelper.h"
#include "Static/FTString.h"

#ifdef FOXTROT_EDITOR
	#include "CommandHistory.h"
#endif

const FTRectArea FTRectArea::Zero(0.0f, 0.0f, 0.0f, 0.0f);

bool FTRectArea::Overlaps(const FTVector2& point)
{
	FTVector2 v1 = FTVector2(Math::Cos(mRotAngle), Math::Sin(mRotAngle));
	FTVector2 v2 = FTVector2(-v1.y, v1.x); // Rotate by 90

	// scale them appropriately by the dimensions
	v1 *= mWidth / 2;
	v2 *= mHeight / 2;

	FTVector2 p0 = mCenter + v1 + v2;
	FTVector2 p1 = mCenter - v1 + v2;
	FTVector2 p2 = mCenter - v1 - v2;
	FTVector2 p3 = mCenter + v1 - v2;

	return Math::PointInRectangle(point, p0, p1, p2, p3);
}

// bool FTRectArea::Overlaps(const FTRectArea& other)
//{
//	FTVector2 rightMin = other.mMin;
//	FTVector2 rightMax = other.mMax;
//	return Overlaps(other.mMin) || Overlaps(other.mMax);
// }

void FTRectArea::Set(FTVector2 center, FTVector2 dimension, float rotAngle)
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
	CommandHistory::GetInstance()->UpdateVector2Value("Center", mCenter);
	CommandHistory::GetInstance()->UpdateVector2Value("Size", mSize);
	CommandHistory::GetInstance()->UpdateFloatValue("Rot Angle", mRotAngle);
	Set(mCenter, mSize, mRotAngle);
}
#endif

FTRectArea* CreateFTRectArea()
{
	return DBG_NEW FTRectArea();
}

void SaveProperties(std::ofstream* ofs, FTRectArea* rectArea)
{
	FileIOHelper::BeginDataPackSave(*ofs, ChunkKey::FTRectArea);
	FileIOHelper::SaveVector2(*ofs, ChunkKey::FTRectArea_CENTER, rectArea->GetCenter());
	FileIOHelper::SaveVector2(*ofs, ChunkKey::FTRectArea_SIZE, rectArea->GetSize());
	FileIOHelper::SaveFloat(*ofs, ChunkKey::FTRectArea_ROTANGLE, rectArea->GetRotAngle());
	FileIOHelper::EndDataPackSave(*ofs, ChunkKey::FTRectArea);
}

void LoadProperties(std::ifstream* ifs, FTRectArea* rectArea)
{
	float	  rotAngle = 0.f;
	FTVector2 size(0.f);
	FTVector2 center(0.f);

	FileIOHelper::BeginDataPackLoad(*ifs);
	FileIOHelper::LoadFloat(*ifs, rotAngle);
	FileIOHelper::LoadVector2(*ifs, size);
	FileIOHelper::LoadVector2(*ifs, center);
	rectArea->Set(center, size, rotAngle);
}
