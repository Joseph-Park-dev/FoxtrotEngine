// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// A class that holds the tilemap data, such as .csv array, tile sizes,
/// number of tiles from a texture pack (tile count on map), etc.
/// </summary>

#pragma once
#include "ResourceSystem/FTResource.h"

#define WSTRING_OFFSET 32
#define MAX_TEXT_LINES 50

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

class Tile;

class FTTileMap :
	public FTResource
{
public:
	void Initialize();
	void ReadCSV(FTDS::String& str);

public:
	Tile* GetTiles() { return mTileMap; }
	Tile& GetTile(size_t row, size_t column);
	UINT  GetMaxCountOnScreenX() { return mMaxCountOnScreenX; }
	UINT  GetMaxCountOnScreenY() { return mMaxCountOnScreenY; }
	UINT  GetMaxCountOnMapX() { return mMaxCountOnMapX; }
	UINT  GetMaxCountOnMapY() { return mMaxCountOnMapY; }

	void SetTiles(Tile* tiles);
	void SetTileWidth(UINT width);
	void SetTileHeight(UINT height);
	void SetMaxCountOnScreenX(UINT xCount);
	void SetMaxCountOnScreenY(UINT yCount);
	void SetMaxCountOnMapX(UINT xCount);
	void SetMaxCountOnMapY(UINT yCount);

public:
	FTTileMap();
	virtual ~FTTileMap() override;

protected:
	void InitializeTile(Tile& tile, UINT column, UINT row, UINT tileNum);

private:
	// These fields need to be read from .chunk files or be modified on the editor.
	FTCSV* mCSV;
	UINT   mTileWidthOnScreen;
	UINT   mTileHeightOnScreen;
	UINT   mMaxCountOnMapX;
	UINT   mMaxCountOnMapY;

	// These fields need to be initialized when the tilemap is constructed.
	Tile* mTileMap;
	UINT  mMaxCountOnScreenX;
	UINT  mMaxCountOnScreenY;

public:
	virtual void SaveProperties(std::ofstream& ofs) override;
	virtual void LoadProperties(std::ifstream& ifs) override;
	virtual void Process(FTCore* coreInst) override;

#ifdef FOXTROT_EDITOR
public:
	virtual void UpdateUI() override;

public:
	virtual void AddRefCount() override;
	virtual void SubtractRefCount() override;
#endif // FOXTROT_EDITOR
};

namespace ChunkKey
{
	namespace TileMap
	{
		constexpr const char* FTTILEMAP		  = "FTTileMap";
		constexpr const char* CSV_KEY		  = "CSV Key";
		constexpr const char* SCREEN_WIDTH	  = "Tile Width On Screen";
		constexpr const char* SCREEN_HEIGHT	  = "Tile Height On Screen";
		constexpr const char* MAP_MAX_COUNT_X = "Max Count On Map X";
		constexpr const char* MAP_MAX_COUNT_Y = "Max Count On Map Y";
	} // namespace TileMap
} // namespace ChunkKey