// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "ResourceSystem/Tile.h"

#include "Renderer/FTRect.h"

Tile::Tile()
    : mRectOnScreen(FTRect(0.0f, 0.0f, 0.0f, 0.0f))
    , mRectOnMap(FTRect(0.0f, 0.0f, 0.0f,0.0f))
{}
