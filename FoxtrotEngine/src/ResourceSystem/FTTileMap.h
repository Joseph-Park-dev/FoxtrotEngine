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
    Tile**  GetTileMap          () { return mTileMap; }
    UINT    GetMaxCountOnScreenX() { return mMaxCountOnScreenX; }
    UINT    GetMaxCountOnScreenY() { return mMaxCountOnScreenY; }

    void    SetTileWidth(UINT width);
    void    SetTileHeight(UINT height);
    void    SetMaxCountOnMapX(UINT xCount);
    void    SetMaxCountOnMapY(UINT yCount);

public:
             FTTileMap();
    virtual ~FTTileMap() override;

private:
    // These fields need to be read from .chunk files or be modified on the editor.
    UINT    mTileWidthOnScreen;
    UINT    mTileHeightOnScreen;
    UINT    mMaxCountOnMapX;
    UINT    mMaxCountOnMapY;

    // These fields need to be initialized when the tilemap is constructed.
    Tile**  mTileMap;
    UINT    mMaxCountOnScreenX;
    UINT    mMaxCountOnScreenY;

private:
    void    InitializeTile(Tile* tile, UINT column, UINT row, UINT tileNum);

public:
    virtual void SaveProperties(std::ofstream& ofs, UINT key) override;
    virtual UINT LoadProperties(std::ifstream& ifs)           override;
};

