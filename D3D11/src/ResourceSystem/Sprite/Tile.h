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
		FTRectArea* GetRectOnMap() { return mRectOnMap; }
		FTRectArea* GetRectOnScreen() { return mRectOnScreen; }

	public:
		Tile();
		~Tile();

	private:
		// Represents the area on the texture to be sampled.
		FTRectArea* mRectOnMap;
		// Represents the area on display screen.
		FTRectArea* mRectOnScreen;
	};
} // namespace D3D11