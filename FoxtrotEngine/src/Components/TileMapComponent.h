// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// This renders FTTilemaps using FTTexture as a texture pack,
/// primitive square as a mesh, and FTTileMap as a .CSV source.
/// </summary>

#pragma once
#include "Components/SpriteRendererComponent.h"

#include <string>

#include "Core/TemplateFunctions.h"
#include "ResourceSystem/Tile.h"
#include "ResourceSystem/MeshConstantData.h"
#include "ResourceSystem/FTTileMap.h";

class FTVector2;

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

protected:
    void InitializeTileMap();

public:
             TileMapComponent(
                 Actor* owner, 
                 int drawOrder   = DEFAULT_DRAWORDER, 
                 int UpdateOrder = DEFAULT_UPDATEORDER
             );
    virtual ~TileMapComponent() override;


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

protected:
            void UpdateCSV();
            void UpdateCSV(UINT& key);
#endif
};