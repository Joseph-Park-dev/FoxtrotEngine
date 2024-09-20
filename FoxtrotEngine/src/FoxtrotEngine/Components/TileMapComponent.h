#pragma once
#include "FoxtrotEngine/Components/SpriteRendererComponent.h"

#include <string>

#include "FoxtrotEngine/Core/TemplateFunctions.h"
#include "FoxtrotEngine/ResourceSystem/Tile.h"
#include "FoxtrotEngine/ResourceSystem/MeshConstantData.h"
#include "FoxtrotEngine/ResourceSystem/FTTileMap.h";

class FTVector2;

#define MAPKEY_SQUARE_GRID "BasicSquareGrid"

class TileMapComponent :
    public SpriteRendererComponent
{
public:
    virtual void Initialize(FTCore* coreInstance)	    override;
    virtual void Update    (float deltaTime)            override;
    virtual void Render    (FoxtrotRenderer* renderer)  override;

public:
             TileMapComponent(
                Actor* owner, 
                int drawOrder = DEFAULT_DRAWORDER, 
                int UpdateOrder = DEFAULT_UPDATEORDER
            );
    virtual ~TileMapComponent() override;

private:
    FTTileMap*      mTileMap;
    UINT            mTileMapKey;

private:
    void InitializeTileMap();

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