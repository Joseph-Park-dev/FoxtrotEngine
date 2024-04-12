#include <SDL2/SDL.h>
#include "Tile.h"
#include "Transform.h"

Tile::Tile()
    : mTileNum(0)
    , mTileIndexX(0)
    , mTileIndexY(0)
    , mMapRect{}
    , mScreenRect{}
{}

Tile::Tile(int width, int height, int tileNumber)
    : mTileNum(0)
    , mTileIndexX(0)
    , mTileIndexY(0)
    , mMapRect{}
    , mScreenRect{}
{}