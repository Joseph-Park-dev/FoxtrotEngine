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
    void ReadCSV();

public:
    Tile**       GetCurrentTileMap() { return mCurrentTileMap; }
    Tile*        GetTile(int x, int y) { return &mCurrentTileMap[x][y]; }
    std::wstring GetName() const override
    {
        return L"TileMapComponent";
    }
    std::string& GetCSVFileMapPath() { return mCSVTileMapPath; }
    
    FTVector2    GetTileSizeOnScreen() { return mTileSizeOnScreen; }
    int          GetMaxCountOnScreenX() { return mMaxCountOnScreenX; }
    int          GetMaxCountOnScreenY() { return mMaxCountOnScreenY; }
    
    FTVector2    GetTileSizeOnMap() { return mTileSizeOnMap; }
    int          GetMaxCountOnMapX() { return mMaxCountOnMapX; }
    int          GetMaxCountOnMapY() { return mMaxCountOnMapY; }

    void         SetCSVTileMapPath(std::string path) { mCSVTileMapPath.assign(path); }
    
    void         SetTileSizeOnScreen(FTVector2 size) { mTileSizeOnScreen = size; }
    void         SetMaxCountOnScreenX(int count) {  mMaxCountOnScreenX = count; }
    void         SetMaxCountOnScreenY(int count) {  mMaxCountOnScreenY = count; }
    
    void         SetTileSizeOnMap(FTVector2 size) { mTileSizeOnMap = size; }
    void         SetMaxCountOnMapX(int count) {  mMaxCountOnMapX = count; }
    void         SetMaxCountOnMapY(int count) {  mMaxCountOnMapY = count; }

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

    // These fields need to be read from .chunk files.
    FTVector2   mTileSizeOnScreen;
    FTVector2   mTileSizeOnMap;

    // These fields need to be initialized when the tilemap is constructed.
    int         mMaxCountOnScreenX;
    int         mMaxCountOnScreenY;
    int         mMaxCountOnMapX;
    int         mMaxCountOnMapY;

private:
    void InitializeTileMap();
    void InitializeTile(Tile* tile, UINT column, UINT row, UINT tileNum);

    //void UpdateMesh(Transform* transform, Camera* cameraInstance) override;
    //void SetScreenRect(FTVector2 worldPos, Tile* tile);

#ifdef _DEBUG
public:
    //virtual void EditorUpdate(float deltaTime) override;
    virtual void EditorUIUpdate() override;

    virtual void SaveProperties(nlohmann::ordered_json& out) override;
    virtual void LoadProperties(std::ifstream& ifs) override;

    virtual void OnConfirmUpdate() override;
    void UpdateCSV();
#endif // _DEBUG
};