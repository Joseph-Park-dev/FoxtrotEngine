#include "FoxtrotEngine/ResourceSystem/Tile.h"

#include "FoxtrotEngine/Actors/Transform.h"
#include "FoxtrotEngine/Renderer/FTRect.h"

Tile::Tile()
    : mTileIndexX(0)
    , mTileIndexY(0)
    , mTileNumber(0)
    , mMapRect(FTRect(0,0,0,0))
{}
