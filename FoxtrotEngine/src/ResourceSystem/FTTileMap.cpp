// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "FTTileMap.h"

#include <fstream>
#include <sstream>
#include <queue>
#include <string>

#include "ResourceSystem/Tile.h"
#include "Renderer/FTRectArea.h"
#include "Core/TemplateFunctions.h"
#include "FileSystem/ChunkLoader.h"
#include "FileSystem/FileIOHelper.h"
#include "FileSystem/BufferSizes.h"
#include "Managers/ResourceManager.h"
#include "ResourceSystem/GenericData/FTCSV.h"

#ifdef FOXTROT_EDITOR
	#include "CommandHistory.h"
	#include "EditorUtils.h"
	#include "EditorResourceManager.h"
#endif // FOXTROT_EDITOR

void FTTileMap::Initialize()
{
	if (!mCSV)
		return;

	// These values cannot be 0;
	assert(mTileWidthOnScreen != 0);
	assert(mTileHeightOnScreen != 0);
	assert(mMaxCountOnMapX != 0);
	assert(mMaxCountOnMapY != 0);

	if (mTileMap)
		delete[] mTileMap;

	UINT column		   = mCSV->GetColumnCount();
	UINT row		   = mCSV->GetRowCount();
	mMaxCountOnScreenX = column;
	mMaxCountOnScreenY = row;

	mTileMap = DBG_NEW Tile[column * row];

	for (int r = 0; r < row; ++r)
	{
		for (int c = 0; c < column; ++c)
		{
			int idx = column * r + c;
			if (mCSV->Data().at(idx))
				InitializeTile(mTileMap[idx], c, r, mCSV->Data().at(idx));
		}
	}
}

void FTTileMap::ReadCSV(FTDS::String& str)
{
	// These values cannot be 0;
	assert(mTileWidthOnScreen != 0);
	assert(mTileHeightOnScreen != 0);
	assert(mMaxCountOnMapX != 0);
	assert(mMaxCountOnMapY != 0);

	if (mTileMap)
		delete[] mTileMap;

	std::ifstream	myFile;
	std::queue<int> result;

	// Open an existing file
	myFile.open(GetRelativePath().C_Str(), std::fstream::in);
	assert(myFile);
	std::string line;
	int			val;
	int			column = 0;
	int			row	   = 0;
	while (std::getline(myFile, line))
	{
		std::stringstream ss(line);
		// Create a stringstream of the current line
		// Extract each integer
		while (ss >> val)
		{
			result.push(val);
			if (ss.peek() == ',')
				ss.ignore();
			++column;
		}
		++row;
	}
	if (row != 0)
		column /= row;
	myFile.close();

	mMaxCountOnScreenX = column;
	mMaxCountOnScreenY = row;

	mTileMap = DBG_NEW Tile[str.Length()];
	for (UINT i = 0; i < (UINT)str.Length(); ++i)
	{
		UINT idx = str.At(i) - WSTRING_OFFSET;
		InitializeTile(mTileMap[i], i, 0, idx);
	}
}

Tile& FTTileMap::GetTile(size_t row, size_t column)
{
	return mTileMap[mMaxCountOnScreenX * row + column];
}

void FTTileMap::SetTiles(Tile* tiles)
{
	mTileMap = tiles;
}

void FTTileMap::SetTileWidth(UINT width)
{
	mTileWidthOnScreen = width;
}

void FTTileMap::SetTileHeight(UINT height)
{
	mTileHeightOnScreen = height;
}

void FTTileMap::SetMaxCountOnScreenX(UINT xCount)
{
	mMaxCountOnScreenX = xCount;
}

void FTTileMap::SetMaxCountOnScreenY(UINT yCount)
{
	mMaxCountOnScreenY = yCount;
}

void FTTileMap::SetMaxCountOnMapX(UINT xCount)
{
	mMaxCountOnMapX = xCount;
}

void FTTileMap::SetMaxCountOnMapY(UINT yCount)
{
	mMaxCountOnMapY = yCount;
}

FTTileMap::FTTileMap(FTResourceDef& resDef)
	: FTResource(resDef)
	, mTileWidthOnScreen(0)
	, mTileHeightOnScreen(0)
	, mMaxCountOnMapX(0)
	, mMaxCountOnMapY(0)
	, mTileMap(nullptr)
	, mMaxCountOnScreenX(0)
	, mMaxCountOnScreenY(0)
{
}

FTTileMap::~FTTileMap()
{
	if (mTileMap)
	{
		delete[] mTileMap;
		mTileMap = nullptr;
	}
}

void FTTileMap::InitializeTile(Tile& tile, UINT column, UINT row, UINT tileNum)
{
	FTRectArea& rectOnMap = tile.GetRectOnMap();
	// Individual Tile size on tilemap
	float tileWidthOnMap  = 1 / static_cast<float>(mMaxCountOnMapX);
	float tileHeightOnMap = 1 / static_cast<float>(mMaxCountOnMapY);

	int tileIndexX = tileNum % mMaxCountOnMapX;
	int tileIndexY = tileNum / mMaxCountOnMapX;
	rectOnMap.Set(tileWidthOnMap * tileIndexX, tileHeightOnMap * tileIndexY, tileWidthOnMap, tileHeightOnMap);

	FTRectArea& rectOnScreen = tile.GetRectOnScreen();
	rectOnScreen.Set(
		static_cast<float>(column * mTileWidthOnScreen),
		static_cast<float>(row * mTileHeightOnScreen),
		static_cast<float>(mTileWidthOnScreen),
		static_cast<float>(mTileHeightOnScreen));
}

void FTTileMap::SaveProperties(std::ofstream& ofs)
{
	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::FTTileMap::FT_TILEMAP);
	FTResource::SaveProperties(ofs);
	FileIOHelper::SaveString(ofs, ChunkKey::FTTileMap::CSV_KEY, mCSV->GetFileName());
	FileIOHelper::SaveUnsignedInt(ofs, ChunkKey::FTTileMap::SCREEN_WIDTH, mTileWidthOnScreen);
	FileIOHelper::SaveUnsignedInt(ofs, ChunkKey::FTTileMap::SCREEN_HEIGHT, mTileHeightOnScreen);
	FileIOHelper::SaveUnsignedInt(ofs, ChunkKey::FTTileMap::MAP_MAX_COUNT_X, mMaxCountOnMapX);
	FileIOHelper::SaveUnsignedInt(ofs, ChunkKey::FTTileMap::MAP_MAX_COUNT_Y, mMaxCountOnMapY);
	FileIOHelper::EndDataPackSave(ofs, ChunkKey::FTTileMap::FT_TILEMAP);
}

void FTTileMap::LoadProperties(std::ifstream& ifs)
{
	FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FTTileMap::FT_TILEMAP);
	FileIOHelper::LoadUnsignedInt(ifs, mMaxCountOnMapY);
	FileIOHelper::LoadUnsignedInt(ifs, mMaxCountOnMapX);
	FileIOHelper::LoadUnsignedInt(ifs, mTileHeightOnScreen);
	FileIOHelper::LoadUnsignedInt(ifs, mTileWidthOnScreen);
	FileIOHelper::LoadResource(ifs, mCSV, ResourceManager::GetInstance()->GetCSVs());
	FTResource::LoadProperties(ifs);
}

void FTTileMap::Process()
{
	if (IsProcessed())
		return;

	// This if statement will be triggered only on Editor
	// (When loading all assets from Asset folder)
	std::ifstream ifs(this->GetRelativePath().C_Str());
	this->LoadProperties(ifs);

	this->Initialize();
	FTResource::Process();
}

#ifdef FOXTROT_EDITOR
void FTTileMap::UpdateUI()
{
	ImVec2 previewSize = ImVec2(100, 100);

	FTEditorUtils::DisplayResSelection(
		"Select CSV",
		EditorResourceManager::GetInstance()->GetCSVs(),
		mCSV);

	int tileWidthOnScreen  = static_cast<int>(mTileWidthOnScreen);
	int tileHeightOnScreen = static_cast<int>(mTileHeightOnScreen);
	int maxCountOnMapX	   = static_cast<int>(mMaxCountOnMapX);
	int maxCountOnMapY	   = static_cast<int>(mMaxCountOnMapY);

	CommandHistory::GetInstance()->UpdateIntValue("Tile width on screen", tileWidthOnScreen);
	CommandHistory::GetInstance()->UpdateIntValue("Tile height on screen", tileHeightOnScreen);
	CommandHistory::GetInstance()->UpdateIntValue("Max count on Map X", maxCountOnMapX);
	CommandHistory::GetInstance()->UpdateIntValue("Max count on Map Y", maxCountOnMapY);

	mTileWidthOnScreen	= static_cast<UINT>(tileWidthOnScreen);
	mTileHeightOnScreen = static_cast<UINT>(tileHeightOnScreen);
	mMaxCountOnMapX		= static_cast<UINT>(maxCountOnMapX);
	mMaxCountOnMapY		= static_cast<UINT>(maxCountOnMapY);
}

void FTTileMap::AddRefCount()
{
	if (mCSV)
		mCSV->AddRefCount();
	FTResource::AddRefCount();
}

void FTTileMap::SubtractRefCount()
{
	if (mCSV)
		mCSV->SubtractRefCount();
	FTResource::SubtractRefCount();
}
#endif