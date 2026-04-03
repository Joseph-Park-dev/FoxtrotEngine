// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "ResourceSystem/Sprite/Tile.h"

#include "Renderer/FTRectArea.h"
#include "Debugging/DebugMemAlloc.h"

namespace D3D11
{
	Tile::Tile()
		: mRectOnScreen(DBG_NEW FTRectArea(0.0f, 0.0f, 0.0f, 0.0f))
		, mRectOnMap(DBG_NEW FTRectArea(0.0f, 0.0f, 0.0f, 0.0f))
	{
	}

	Tile::~Tile()
	{
		delete mRectOnScreen;
		delete mRectOnMap;
	}
} // namespace D3D11