#pragma once
#include "FoxtrotEngine/Components/SpriteRendererComponent.h"

#include <string>

#include "FoxtrotEngine/Core/TemplateFunctions.h"
#include "FoxtrotEngine/ResourceSystem/Tile.h"

class FTVector2;

class TileMapComponent :
    public SpriteRendererComponent
{
public:
    virtual void Initialize(FTCore* coreInstance)	override;
    virtual void Update(float deltaTime)            override;
    virtual void Render(FoxtrotRenderer* renderer)  override;
    //void RenderEX(FoxtrotRenderer* renderer);
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

    void         SetCSVTileMapPath(std::string path) { mCSVTileMapPath = path; }
    void         SetTileSize(FTVector2 size) { mTileSizeX = size.x; mTileSizeY = size.y; }

public:
    TileMapComponent(Actor* owner, int drawOrder = DEFAULT_DRAWORDER, int UpdateOrder = DEFAULT_UPDATEORDER);
    virtual ~TileMapComponent() override;

private:
    Tile**      mCurrentTileMap;
    int         mTileCountX;
    int         mTileCountY;
    // Individual tile size
    int         mTileSizeX;
    int         mTileSizeY;
    std::string mCSVTileMapPath;

private:
    void InitializeTileMap();
    void InitializeTile(Tile* tile, UINT indexX, UINT indexY);
    void ReadCSV();

    //void UpdateMesh(Transform* transform, Camera* cameraInstance) override;
    //void SetScreenRect(FTVector2 worldPos, Tile* tile);

#ifdef _DEBUG
public:
    virtual void SaveProperties(nlohmann::ordered_json& out) override;
    virtual void LoadProperties(std::ifstream& ifs) override;

    //void BlitToGameviewEx(FTVector2 wPos, Tile* tile);
    //void BlitToGameview(FTVector2 worldPos, Tile* tile);

private:
    //void SetScreenRectEditorView(FTVector2 editorPos, Tile* tile);
#endif // _DEBUG
};