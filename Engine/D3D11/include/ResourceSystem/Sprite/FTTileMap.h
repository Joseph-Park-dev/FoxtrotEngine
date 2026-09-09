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
#include "FTCSV.h"
#include "ResourceSystem/D3D11Resource.h"

#define WSTRING_OFFSET 32
#define MAX_TEXT_LINES 50

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "ResourceSystem/ResourcePack.h"

namespace Core
{
	using FTCSV = GenericData::FTCSV;
}

namespace D3D11
{
	using namespace Common;
	class Tile;

	using GET_CSVS_FUNC = Common::ResourcePack<Core::FTCSV>* (*)();
	class FTTileMap :
		public D3D11Resource
	{
	public:
		static D3D11::ResType Type;

	public:
		/// @brief Initializes the services and state required before this object's runtime lifecycle begins.
		void Initialize();
		/// @brief Parses comma-separated integer data into tile-map cells.
		/// @param resDef Resource definition containing the filename and source path.
		/// @param str Text used by the operation.
		void ReadCSV(Common::FTResourceDef& resDef, Common::FTDS::String& str);

	public:
		/// @brief Returns the tiles used by this fttile map.
		/// @return Borrowed access to the tiles.
		Tile* GetTiles() { return mTileMap; }
		/// @brief Returns the tile used by this fttile map.
		/// @param row Row index in the grid.
		/// @param column Column index in the grid.
		/// @return Borrowed access to the tile.
		/// @note Changes through the returned reference affect this object's stored state.
		Tile& GetTile(size_t row, size_t column);
		/// @brief Returns the max count on screen x used by this fttile map.
		/// @return Current max count on screen x.
		UINT  GetMaxCountOnScreenX() { return mMaxCountOnScreenX; }
		/// @brief Returns the max count on screen y used by this fttile map.
		/// @return Current max count on screen y.
		UINT  GetMaxCountOnScreenY() { return mMaxCountOnScreenY; }
		/// @brief Returns the max count on map x used by this fttile map.
		/// @return Current max count on map x.
		UINT  GetMaxCountOnMapX() { return mMaxCountOnMapX; }
		/// @brief Returns the max count on map y used by this fttile map.
		/// @return Current max count on map y.
		UINT  GetMaxCountOnMapY() { return mMaxCountOnMapY; }

		/// @brief Updates the tiles used by subsequent operations.
		/// @param tiles Replacement tiles.
		void SetTiles(Tile* tiles);
		/// @brief Updates the tile width used by subsequent operations.
		/// @param width Replacement tile width.
		void SetTileWidth(UINT width);
		/// @brief Updates the tile height used by subsequent operations.
		/// @param height Replacement tile height.
		void SetTileHeight(UINT height);
		/// @brief Updates the max count on screen x used by subsequent operations.
		/// @param xCount Replacement max count on screen x.
		void SetMaxCountOnScreenX(UINT xCount);
		/// @brief Updates the max count on screen y used by subsequent operations.
		/// @param yCount Replacement max count on screen y.
		void SetMaxCountOnScreenY(UINT yCount);
		/// @brief Updates the max count on map x used by subsequent operations.
		/// @param xCount Replacement max count on map x.
		void SetMaxCountOnMapX(UINT xCount);
		/// @brief Updates the max count on map y used by subsequent operations.
		/// @param yCount Replacement max count on map y.
		void SetMaxCountOnMapY(UINT yCount);

	public:
		/// @brief Initializes tile-map dimensions and tile storage.
		/// @param resDef Resource definition containing the filename and source path.
		FTTileMap(Common::FTResourceDef& resDef);
		/// @brief Releases the resources managed by this instance during destruction.
		~FTTileMap();

	protected:
		/// @brief Initializes the tile's geometry and texture coordinates.
		/// @param tile Tile instance to process.
		/// @param column Column index in the grid.
		/// @param row Row index in the grid.
		/// @param tileNum Tile index or number of tiles.
		void InitializeTile(Tile& tile, size_t column, size_t row, size_t tileNum);

	private:
		// These fields need to be read from .chunk files or be modified on the editor.
		Core::FTCSV* mCSV;
		UINT		 mTileWidthOnScreen;
		UINT		 mTileHeightOnScreen;
		UINT		 mMaxCountOnMapX;
		UINT		 mMaxCountOnMapY;

		// These fields need to be initialized when the tilemap is constructed.
		Tile* mTileMap;
		UINT  mMaxCountOnScreenX;
		UINT  mMaxCountOnScreenY;

	public:
		/// @brief Serializes this object's persistent properties to a .chunk stream.
		/// @param ofs Output stream receiving the serialized data.
		/// @note Writes to the supplied stream at its current position.
		virtual void SaveProperties(std::ofstream& ofs) override;
		/// @brief Restores this object's persistent properties from a .chunk stream.
		/// @param ifs Input stream positioned at the expected data; reading advances its position.
		/// @note Advances the stream position and updates the destination state.
		virtual void LoadProperties(std::ifstream& ifs) override;

#ifdef FOXTROT_EDITOR
	public:
		/// @brief Builds the editor controls for inspecting and modifying this object's state.
		virtual void UpdateUI() override;

	public:
		/// @brief Increments the resource metadata's reference count when metadata exists.
		virtual void AddRefCount() override;
		/// @brief Decrements the resource metadata's reference count when metadata exists.
		virtual void SubtractRefCount() override;

	private:
		GET_CSVS_FUNC mGetCSVsFunc;
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
