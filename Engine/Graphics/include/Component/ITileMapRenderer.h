// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// This renders FTTilemaps using FTTexture as a texture pack,
/// primitive square as a mesh, and FTTileMap as a .CSV source.
/// </summary>

#pragma once
#include "ISpriteRenderer.h"

#include "FTDS/Static/FTString.h"

namespace Graphics
{
	class ITileMap;

	class ITileMapRenderer :
		public ISpriteRenderer
	{
	public:
		/// @brief Returns the tile map key used by this itile map renderer.
		/// @return Borrowed access to the tile map key.
		virtual const char* GetTileMapKey()	   = 0;
		/// @brief Returns the tile map used by this itile map renderer.
		/// @return Borrowed access to the tile map.
		virtual ITileMap*	GetTileMap() const = 0;

		/// @brief Updates the tile map key used by subsequent operations.
		/// @param key Replacement tile map key.
		virtual void SetTileMapKey(const char* key) = 0;
		/// @brief Updates the tile map used by subsequent operations.
		/// @param tileMap Replacement tile map.
		virtual void SetTileMap(ITileMap* tileMap)	= 0;

	protected:
		/// @brief Builds the tile-map data and geometry used for rendering.
		virtual void InitializeTileMap() = 0;
	};

	namespace ChunkKey
	{
		constexpr const char* TILEMAP_KEY = "TileMapKey";
	}
} // namespace Graphics
