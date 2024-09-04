#pragma once
#include "FoxtrotEngine/Components/SpriteRendererComponent.h"

#include <string>

#include "FoxtrotEngine/Core/TemplateFunctions.h"
#include "FoxtrotEngine/ResourceSystem/Tile.h"
#include "FoxtrotEngine/ResourceSystem/MeshConstantData.h"

class FTVector2;

#define MAPKEY_SQUARE_GRID "BasicSquareGrid"

class TileMapComponent :
    public SpriteRendererComponent
{
public:
    virtual void Initialize(FTCore* coreInstance)	    override;
    virtual void Update    (float deltaTime)            override;
    virtual void Render    (FoxtrotRenderer* renderer)  override;
    //void RenderEX(FoxtrotRenderer* renderer);
    //void DrawIndividualTileOnPos(FoxtrotRenderer* renderer, FTVector2 worldPos, Tile* tile);
    //void DrawIndividualTileOnPosEX(FoxtrotRenderer* renderer, FTVector2 worldPos, Tile* tile);

public:
    Tile**       GetCurrentTileMap() { return mCurrentTileMap; }
    Tile*        GetTile(int x, int y) { return &mCurrentTileMap[x][y]; }
    std::wstring GetName() const override
    {
        return L"TileMapComponent";
    }

    void         SetCSVTileMapPath(std::string path) { mCSVTileMapPath.assign(path); }

public:
            TileMapComponent(
                Actor* owner, 
                int drawOrder = DEFAULT_DRAWORDER, 
                int UpdateOrder = DEFAULT_UPDATEORDER
            );
    virtual ~TileMapComponent() override;

private:
    TileMapConstantData mTileMapConstantData;

    Tile**      mCurrentTileMap;
    std::string mCSVTileMapPath;

    int         mTileSizeOnScreenX;
    int         mTileSizeOnScreenY;
    int         mMaxCountOnScreenX;
    int         mMaxCountOnScreenY;

    int         mTileSizeOnMapX;
    int         mTileSizeOnMapY;
    int         mMaxCountOnMapX;
    int         mMaxCountOnMapY;

private:
    void InitializeTileMap();
    void InitializeTile(Tile* tile, UINT column, UINT row, UINT tileNum);
    void ReadCSV();

    //void UpdateMesh(Transform* transform, Camera* cameraInstance) override;
    //void SetScreenRect(FTVector2 worldPos, Tile* tile);

#ifdef _DEBUG
public:
    //virtual void EditorUpdate(float deltaTime) override;
    virtual void EditorUIUpdate() override;

    virtual void SaveProperties(nlohmann::ordered_json& out) override;
    virtual void LoadProperties(std::ifstream& ifs) override;

    void UpdateCSV();

    //virtual void EditorRender(FoxtrotRenderer* renderer) override;
    //void BlitToGameviewEx(FTVector2 wPos, Tile* tile);
    //void BlitToGameview(FTVector2 worldPos, Tile* tile);

private:
    //void SetScreenRectEditorView(FTVector2 editorPos, Tile* tile);
#endif // _DEBUG
};