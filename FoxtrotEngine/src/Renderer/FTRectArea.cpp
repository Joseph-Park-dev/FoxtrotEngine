// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Renderer/FTRectArea.h"
#include "FileSystem/FileIOHelper.h"

#ifdef FOXTROT_EDITOR
	#include "CommandHistory.h"
#endif

const FTRectArea FTRectArea::Zero(0.0f, 0.0f, 0.0f, 0.0f);

bool FTRectArea::Overlaps(const FTVector2& point)
{
	return mMin.x <= point.x && point.x <= mMax.x && mMin.y <= point.y && point.y <= mMax.y;
}

bool FTRectArea::Overlaps(const FTRectArea& other)
{
	FTVector2 rightMin = other.mMin;
	FTVector2 rightMax = other.mMax;
	return Overlaps(other.mMin) || Overlaps(other.mMax);
}

void FTRectArea::Set(FTVector2 center, FTVector2 dimension)
{
	mCenter = center;
	mSize	= dimension;
	mWidth	= dimension.x;
	mHeight = dimension.y;
	mMin	= mCenter - mSize / 2;
	mMax	= mCenter + mSize / 2;
}

void FTRectArea::Set(float posX, float posY, float width, float height)
{
	mWidth	= width;
	mHeight = height;
	mSize	= FTVector2(width, height);
	mCenter = FTVector2(posX, posY) + mSize * 0.5f;
	mMin	= mCenter - mSize / 2;
	mMax	= mCenter + mSize / 2;
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

void FTRectArea::SaveProperties(std::ofstream& ofs)
{
	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::FTRectArea);
	FileIOHelper::SaveVector2(ofs, ChunkKey::FTRectArea_CENTER, mCenter);
	FileIOHelper::SaveVector2(ofs, ChunkKey::FTRectArea_SIZE, mSize);
	FileIOHelper::EndDataPackSave(ofs, ChunkKey::FTRectArea);
}

void FTRectArea::LoadProperties(std::ifstream& ifs)
{
	FileIOHelper::BeginDataPackLoad(ifs);
	FileIOHelper::LoadVector2(ifs, mSize);
	FileIOHelper::LoadVector2(ifs, mCenter);
	Set(mCenter, mSize);
}

#ifdef FOXTROT_EDITOR
void FTRectArea::UpdateUI()
{
	CommandHistory::GetInstance()->UpdateVector2Value("Center", mCenter);
	CommandHistory::GetInstance()->UpdateVector2Value("Size", mSize);
	Set(mCenter, mSize);
}
#endif