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
		virtual const char* GetTileMapKey()	   = 0;
		virtual ITileMap*	GetTileMap() const = 0;

		virtual void SetTileMapKey(const char* key) = 0;
		virtual void SetTileMap(ITileMap* tileMap)	= 0;

	protected:
		virtual void InitializeTileMap() = 0;
	};

	namespace ChunkKey
	{
		constexpr const char* TILEMAP_KEY = "TileMapKey";
	}
} // namespace Graphics