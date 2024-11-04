#include "ResourceSystem/Tile.h"

#include "Actors/Transform.h"
#include "Renderer/FTRect.h"

Tile::Tile()
    : mRectOnScreen(FTRect(0.0f, 0.0f, 0.0f, 0.0f))
    , mRectOnMap(FTRect(0.0f, 0.0f, 0.0f,0.0f))
{}
