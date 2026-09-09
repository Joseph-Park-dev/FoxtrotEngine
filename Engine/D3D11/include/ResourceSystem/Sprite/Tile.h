// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Defines a single tile data that forms a FTTileMap.
/// </summary>

#pragma once
#include "Renderer/FTRectArea.h"

namespace D3D11
{
	class Tile
	{
	public:
		/// @brief Returns the rect on map used by this tile.
		/// @return Borrowed access to the rect on map.
		D3D11::FTRectArea* GetRectOnMap() { return mRectOnMap; }
		/// @brief Returns the rect on screen used by this tile.
		/// @return Borrowed access to the rect on screen.
		D3D11::FTRectArea* GetRectOnScreen() { return mRectOnScreen; }

	public:
		/// @brief Initializes tile placement and texture selection.
		Tile();
		/// @brief Releases the resources managed by this instance during destruction.
		~Tile();

	private:
		// Represents the area on the texture to be sampled.
		D3D11::FTRectArea* mRectOnMap;
		// Represents the area on display screen.
		D3D11::FTRectArea* mRectOnScreen;
	};
} // namespace D3D11
