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
#include "Renderer/FTRect.h"

class Tile
{
public:
    FTRect& GetRectOnMap() { return mRectOnMap; }
    FTRect& GetRectOnScreen() { return mRectOnScreen; }

public:
    Tile();

private:
    // Represents the area on the texture to be sampled.
    FTRect mRectOnMap;
    // Represents the area on display screen.
    FTRect mRectOnScreen; 
};

