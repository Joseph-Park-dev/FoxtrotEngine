// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Renderer/FTRect.h"
#include "FileSystem/FileIOHelper.h"

#ifdef FOXTROT_EDITOR
	#include "CommandHistory.h"
#endif

const FTRect FTRect::Zero(0.0f, 0.0f, 0.0f, 0.0f);

bool FTRect::Overlaps(const FTVector2& point)
{
	return mMin.x <= point.x && point.x <= mMax.x && mMin.y <= point.y && point.y <= mMax.y;
}

bool FTRect::Overlaps(const FTRect& other)
{
	FTVector2 rightMin = other.mMin;
	FTVector2 rightMax = other.mMax;
	return Overlaps(other.mMin) || Overlaps(other.mMax);
}

void FTRect::SetPosition(FTVector2 val)
{
	mCenter = val;
}

void FTRect::SetSize(FTVector2 val)
{
	mSize = val;
}

void FTRect::Set()
{
	Set(mCenter, mSize);
}

void FTRect::Set(FTVector2 center, FTVector2 dimension)
{
	mCenter = center;
	mSize	= dimension;
	mWidth	= dimension.x;
	mHeight = dimension.y;
	mMin	= mCenter - mSize / 2;
	mMax	= mCenter + mSize / 2;
}

void FTRect::Set(float posX, float posY, float width, float height)
{
	mMin	= FTVector2(posX, posY);
	mWidth	= width;
	mHeight = height;
	mSize	= FTVector2(width, height);
	mCenter = mMin + mSize / 2;
	mMax	= mMin + mSize;
}

void FTRect::CloneTo(FTRect* rect)
{
	rect->mCenter = mCenter;
	rect->mSize	  = mSize;
	Set();
}

void FTRect::SaveProperties(std::ofstream& ofs)
{
	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::FTRECT);
	FileIOHelper::SaveVector2(ofs, ChunkKey::FTRECT_CENTER, mCenter);
	FileIOHelper::SaveVector2(ofs, ChunkKey::FTRECT_SIZE, mSize);
	FileIOHelper::EndDataPackSave(ofs, ChunkKey::FTRECT);
}

void FTRect::LoadProperties(std::ifstream& ifs)
{
	FileIOHelper::BeginDataPackLoad(ifs);
	FileIOHelper::LoadVector2(ifs, mSize);
	FileIOHelper::LoadVector2(ifs, mCenter);
	Set();
}

#ifdef FOXTROT_EDITOR
void FTRect::UpdateUI()
{
	CommandHistory::GetInstance()->UpdateVector2Value("Center", mCenter);
	CommandHistory::GetInstance()->UpdateVector2Value("Size", mSize);
	Set();
}
#endif