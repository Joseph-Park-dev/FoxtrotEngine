#pragma once
#include "Components/SpriteRendererComponent.h"

#include <string>

#include "Core/TemplateFunctions.h"
#include "ResourceSystem/Tile.h"
#include "ResourceSystem/MeshConstantData.h"
#include "ResourceSystem/FTTileMap.h";

class FTVector2;

#define MAPKEY_SQUARE_GRID "BasicSquareGrid"

class TileMapComponent :
    public SpriteRendererComponent
{
public:
    std::string GetName()		const override {
        return	"TileMapComponent";
    }
    UINT        GetTileMapKey() const;
    FTTileMap*  GetTileMap() const;

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

protected:
    void InitializeTileMap();

private:
    FTTileMap*      mTileMap;
    UINT            mTileMapKey;

public:
    virtual void SaveProperties(std::ofstream& ofs) override;
    virtual void LoadProperties(std::ifstream& ifs) override;

#ifdef FOXTROT_EDITOR
public:
    virtual void EditorUIUpdate() override;
    virtual void CloneTo(Actor* actor)	override;

    virtual void OnConfirmUpdate() override;
    void UpdateCSV();
    void UpdateCSV(UINT& key);
#endif
};