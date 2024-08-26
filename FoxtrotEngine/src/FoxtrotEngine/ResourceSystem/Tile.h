#pragma once
#include "FoxtrotEngine/Renderer/FTRect.h"

class Tile
{
public:
    FTRect& GetMapRect()      { return mMapRect; }
    int  GetTileNumber()   { return mTileNumber; }
    int  GetTileIndexX()   { return mTileIndexX; }
    int  GetTileIndexY()   { return mTileIndexY; }

    void  SetRect(FTRect rect)            { mMapRect = rect; }
    void  SetTileNumber(int number)    { mTileNumber = number; }
    void  SetTileIndexX(int index)     { mTileIndexX = index; }
    void  SetTileIndexY(int index)     { mTileIndexY = index; }

public:
    Tile();

private:
    // Tile rect value on tilemap
    FTRect mMapRect;
    // Tile rect display on screen
    // Tile value on tilemap
    int mTileNumber;
    // Tile indexes on display screen
    int mTileIndexX;
    int mTileIndexY;
};

