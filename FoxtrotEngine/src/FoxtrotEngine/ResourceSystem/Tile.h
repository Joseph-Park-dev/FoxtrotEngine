#pragma once
#include "FoxtrotEngine/Renderer/FTRect.h"

class Tile
{
public:
    FTRect*     GetRectOnScreen() { return mRectOnScreen; }
    FTRect*     GetRectOnMap()    { return mRectOnMap; }
    //const int&  GetIndexOnMapX() const { return mIndexOnMapX; }
    //const int&  GetIndexOnMapY() const { return mIndexOnMapY; }
    //const int&  GetIndexOnScreenX() const { return mIndexOnScreenX; }
    //const int&  GetIndexOnScreenY() const { return mIndexOnScreenY; }

    //void  SetTileNumber(int number)    { mTileNumber = number; }
    //void  SetTileIndexX(int index)     { mTileIndexX = index; }
    //void  SetTileIndexY(int index)     { mTileIndexY = index; }

public:
    Tile();
    ~Tile();

private:
    FTRect* mRectOnMap;
    //int mIndexOnMapX;    // Tile value on tilemap
    //int mIndexOnMapY;

    FTRect* mRectOnScreen;    // Represents the area on display screen
    // Tile indexes on display screen
    //int mIndexOnScreenX;
    //int mIndexOnScreenY;
};

