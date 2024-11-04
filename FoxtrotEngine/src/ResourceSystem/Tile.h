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

