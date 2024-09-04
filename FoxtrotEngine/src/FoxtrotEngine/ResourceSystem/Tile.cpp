#include "FoxtrotEngine/ResourceSystem/Tile.h"

#include "FoxtrotEngine/Actors/Transform.h"
#include "FoxtrotEngine/Renderer/FTRect.h"

Tile::Tile()
    : mRectOnScreen(new FTRect(0.0f, 0.0f, 0.0f, 0.0f))
    , mRectOnMap(new FTRect(0.0f, 0.0f, 0.0f,0.0f))
{}

Tile::~Tile()
{
    delete mRectOnScreen;
    delete mRectOnMap;
}
