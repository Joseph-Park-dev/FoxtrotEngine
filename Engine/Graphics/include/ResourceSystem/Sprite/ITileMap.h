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
#include "IResource.h"

#define WSTRING_OFFSET 32
#define MAX_TEXT_LINES 50

namespace Graphics
{
	class Tile;

	class ITileMap :
		public Common::IResource
	{
		//////////////////////
		/// Initialization ///
		//////////////////////
	public:
		virtual void ReadCSV(const char* csvPath, Common::FTDS::String& out) = 0;

	protected:
		virtual void InitializeTile(Tile& tile, size_t column, size_t row, size_t tileNum) = 0;

		/////////////////
		/// Chunk I/O ///
		/////////////////
	public:
		/// @see FTResource::SaveProperties()
		virtual void SaveProperties(std::ofstream& ofs) override = 0;

		/// @see FTResource::LoadProperties()
		virtual void LoadProperties(std::ifstream& ifs) override = 0;

		///////////////////////////////////
		/// Const/Destructors & Copying ///
		///////////////////////////////////
	public:
		virtual ~ITileMap() override = 0;
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
} // namespace Graphics