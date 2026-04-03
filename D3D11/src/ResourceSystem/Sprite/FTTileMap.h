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
#include "ResourceSystem/D3D11Resource.h"

#define WSTRING_OFFSET 32
#define MAX_TEXT_LINES 50

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

class Tile;
class FTCSV;

namespace D3D11
{
	class FTTileMap :
		public D3D11Resource
	{
	public:
		void Initialize();
		void ReadCSV(FTResourceDef& resDef, FTDS::String& str);

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
		FTTileMap(FTResourceDef& resDef);
		~FTTileMap();

	protected:
		void InitializeTile(Tile& tile, size_t column, size_t row, size_t tileNum);

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
		namespace FTTileMap
		{
			constexpr const char* FT_TILEMAP	  = "FTTileMap";
			constexpr const char* CSV_KEY		  = "CSV Key";
			constexpr const char* SCREEN_WIDTH	  = "Tile Width On Screen";
			constexpr const char* SCREEN_HEIGHT	  = "Tile Height On Screen";
			constexpr const char* MAP_MAX_COUNT_X = "Max Count On Map X";
			constexpr const char* MAP_MAX_COUNT_Y = "Max Count On Map Y";
		} // namespace FTTileMap
	} // namespace ChunkKey
} // namespace D3D11