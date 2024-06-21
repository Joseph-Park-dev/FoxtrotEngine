#pragma once
#include "FoxtrotEngine/Components/SpriteComponent.h"

#include <string>

#include "FoxtrotEngine/Core/TemplateFunctions.h"
#include "FoxtrotEngine/ResourceSystem/Tile.h"

class FTVector2;

class TileMapComponent :
    public SpriteComponent
{
public:
    void InitializeTileMap(const std::wstring& fileName, int tileSizeX, int tileSizeY);
    void Render(FoxtrotRenderer* renderer);
    void RenderEX(FoxtrotRenderer* renderer);
    //void DrawIndividualTileOnPos(FoxtrotRenderer* renderer, FTVector2 worldPos, Tile* tile);
    //void DrawIndividualTileOnPosEX(FoxtrotRenderer* renderer, FTVector2 worldPos, Tile* tile);

public:
    Tile**       GetCurrentTileMap() { return mCurrentTileMap; }
    Tile*        GetTile(int x, int y) { return &mCurrentTileMap[x][y]; }
    int          GetTileCountX() { return mTileCountX; }
    int          GetTileCountY() { return mTileCountY; }
    std::wstring GetName() const override
    {
        return L"TileMapComponent";
    }

public:
    TileMapComponent(Actor* owner, int drawOrder = DEFAULT_DRAWORDER, int UpdateOrder = DEFAULT_UPDATEORDER);
    virtual ~TileMapComponent() override;

private:
    Tile** mCurrentTileMap;
    int     mTileCountX;
    int     mTileCountY;

private:
    void InitializeTile(Tile* tile);
    void ReadCSV(const std::wstring& fileName, int tileSizeX, int tileSizeY);

    //void SetScreenRect(FTVector2 worldPos, Tile* tile);

#ifdef _DEBUG
public:
    virtual void SaveProperties(std::ofstream& ofs) override;
    virtual void LoadProperties(std::ifstream& ifs) override;

    //void BlitToGameviewEx(FTVector2 wPos, Tile* tile);
    //void BlitToGameview(FTVector2 worldPos, Tile* tile);

private:
    void SetScreenRectEditorView(FTVector2 editorPos, Tile* tile);
#endif // _DEBUG
};