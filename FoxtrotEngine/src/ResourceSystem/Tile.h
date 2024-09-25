#pragma once
#include "Renderer/FTRect.h"

class Tile
{
public:
    FTRect*     GetRectOnScreen() { return mRectOnScreen; }
    FTRect*     GetRectOnMap()    { return mRectOnMap; }

public:
    Tile();
    ~Tile();

private:
    // Represents the area on the texture to be sampled.
    FTRect* mRectOnMap;
    // Represents the area on display screen.
    FTRect* mRectOnScreen; 
};

