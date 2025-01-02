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
#include "ResourceSystem/FTResource.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

class Tile;

class FTTileMap :
    public FTResource
{
public:
    void    ReadCSV();

public:
    Tile*   GetTiles() { return mTileMap; }
    Tile&   GetTile(size_t row, size_t column);
    UINT    GetMaxCountOnScreenX() { return mMaxCountOnScreenX; }
    UINT    GetMaxCountOnScreenY() { return mMaxCountOnScreenY; }
    UINT    GetMaxCountOnMapX() { return mMaxCountOnMapX; }
    UINT    GetMaxCountOnMapY() { return mMaxCountOnMapY; }

    void    SetTileWidth(UINT width);
    void    SetTileHeight(UINT height);
    void    SetMaxCountOnMapX(UINT xCount);
    void    SetMaxCountOnMapY(UINT yCount);

public:
             FTTileMap();
    virtual ~FTTileMap() override;

private:
    // These fields need to be read from .chunk files or be modified on the editor.
    UINT        mTileWidthOnScreen;
    UINT        mTileHeightOnScreen;
    UINT        mMaxCountOnMapX;
    UINT        mMaxCountOnMapY;

    // These fields need to be initialized when the tilemap is constructed.
    Tile*   mTileMap;
    UINT    mMaxCountOnScreenX;
    UINT    mMaxCountOnScreenY;

private:
    void    InitializeTile(Tile& tile, UINT column, UINT row, UINT tileNum);

public:
    virtual void SaveProperties(std::ofstream& ofs, UINT key) override;
    virtual UINT LoadProperties(std::ifstream& ifs)           override;

#ifdef FOXTROT_EDITOR
public:
    virtual void UpdateUI()         override;
#endif // FOXTROT_EDITOR
};

