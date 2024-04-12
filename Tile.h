#pragma once
class Tile
{
public:
    SDL_Rect* GetMapRect()      { return &mMapRect; }
    SDL_Rect* GetScreenRect()   { return &mScreenRect; }
    float     GetWidth()        { return mMapRect.w; }
    float     GetHeight()       { return mMapRect.h; }
    Vector2   GetPosOnMap()     { return Vector2(mMapRect.x, mMapRect.y); }
    int       GetPosOnMapX()    { return mMapRect.x; }
    int       GetPosOnMapY()    { return mMapRect.y; }
    int       GetTileNum()      { return mTileNum; }
    int       GetIndexX()       { return mTileIndexX; }
    int       GetIndexY()       { return mTileIndexY; }

    Vector2   GetPosOnScreen()  { return Vector2(mScreenRect.x, mScreenRect.y); }
    Vector2   GetScale()        { return Vector2(mScreenRect.w, mScreenRect.h); }

    void    SetMapRect(SDL_Rect rect)    { mMapRect = rect; }
    void    SetScreenRect(SDL_Rect rect) { mScreenRect = rect; }
    void    SetWidth(int width)          { mMapRect.w = width; }
    void    SetHeight(int height)        { mMapRect.h = height; }
    void    SetPosOnMap(Vector2 pos) 
    { 
        mMapRect.x = static_cast<int>(pos.x);
        mMapRect.y = static_cast<int>(pos.y);
    }
    void    SetPosOnMapX(int posX)      { mMapRect.x = posX; }
    void    SetPosOnMapY(int posY)      { mMapRect.y = posY; }
    void    SetTileNum(int number)      { mTileNum = number; }
    void    SetIndexX(int number)       { mTileIndexX = number; }
    void    SetIndexY(int number)       { mTileIndexY = number; }
    void    SetPosOnScreen (Vector2 pos)
    {
        mScreenRect.x = static_cast<int>(pos.x);
        mScreenRect.y = static_cast<int>(pos.y);
    }
    void    SetScale(Vector2 scale)
    {
        mScreenRect.w = static_cast<int>(scale.x);
        mScreenRect.h = static_cast<int>(scale.y);
    }

public:
    Tile();
    Tile(int width, int height, int tileNumber);

private:
    // Tile rect value on tilemap
    SDL_Rect mMapRect;
    // Tile rect display on screen
    SDL_Rect mScreenRect;
    // Tile value on tilemap
    int mTileNum;
    // Tile indexes on display screen
    int mTileIndexX;
    int mTileIndexY;
};

