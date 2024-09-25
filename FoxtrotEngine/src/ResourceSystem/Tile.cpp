#include "ResourceSystem/Tile.h"

#include "Actors/Transform.h"
#include "Renderer/FTRect.h"

Tile::Tile()
    : mRectOnScreen(new FTRect(0.0f, 0.0f, 0.0f, 0.0f))
    , mRectOnMap(new FTRect(0.0f, 0.0f, 0.0f,0.0f))
{}

Tile::~Tile()
{
    delete mRectOnScreen;
    delete mRectOnMap;
}
