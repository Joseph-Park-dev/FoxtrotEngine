#pragma once
#include "ResourceSystem/FTResource.h"

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

#ifdef _DEBUG
public:
    virtual void SaveProperties(nlohmann::ordered_json& out, UINT key) override;

    virtual void UpdateUI() override;

#endif // _DEBUG
};

