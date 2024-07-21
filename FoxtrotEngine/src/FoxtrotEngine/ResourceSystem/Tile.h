#pragma once
#include "FoxtrotEngine/Renderer/Rect.h"

class Tile
{
public:
    Rect& GetMapRect()      { return mMapRect; }
    int  GetTileNumber()   { return mTileNumber; }
    int  GetTileIndexX()   { return mTileIndexX; }
    int  GetTileIndexY()   { return mTileIndexY; }

    void  SetRect(Rect rect)            { mMapRect = rect; }
    void  SetTileNumber(int number)    { mTileNumber = number; }
    void  SetTileIndexX(int index)     { mTileIndexX = index; }
    void  SetTileIndexY(int index)     { mTileIndexY = index; }

public:
    Tile();
    Tile(int width, int height, int tileNumber);

private:
    // Tile rect value on tilemap
    Rect mMapRect;
    // Tile rect display on screen
    // Tile value on tilemap
    int mTileNumber;
    // Tile indexes on display screen
    int mTileIndexX;
    int mTileIndexY;
};

