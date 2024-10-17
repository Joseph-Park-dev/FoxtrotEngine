#include "Components/TileMapComponent.h"

#include <queue>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "Actors/Actor.h"
#include "Math/FTMath.h"
#include "ResourceSystem/Tile.h"
#include "ResourceSystem/FTTexture.h"

#include "Actors/Transform.h"
#include "Core/FTCore.h"
#include "ResourceSystem/Mesh.h"
#include "ResourceSystem/FTBasicMeshGroup.h"
#include "ResourceSystem/GeometryGenerator.h"
#include "Renderer/Camera.h"
#include "Renderer/FoxtrotRenderer.h"
#include "Managers/ResourceManager.h"
#include "Renderer/FTRect.h"
#include "FileSystem/ChunkLoader.h"
#include "FileSystem/ChunkFileKeys.h"

#ifdef FOXTROT_EDITOR
#include "CommandHistory.h"
#endif 

#define DEFAULT_TILE_POS 0

void TileMapComponent::Initialize(FTCore* coreInstance){
    MeshRendererComponent::Initialize(coreInstance);
    //SetTexKey("Edit here");
}

void TileMapComponent::InitializeTileMap() {
    assert(mTileMapKey != VALUE_NOT_ASSIGNED);
    assert(GetTexKey() != VALUE_NOT_ASSIGNED);
    assert(GetMeshKey() != VALUE_NOT_ASSIGNED);
    mTileMap = ResourceManager::GetInstance()->GetLoadedTileMap(mTileMapKey);
    if (mTileMap)
    {
        SetTexture();
        mTileMap->ReadCSV();
        ResourceManager::GetInstance()->LoadBasicMesh(
            GeometryGenerator::MakeTileMapGrid(mTileMap)
        );
        MeshRendererComponent::InitializeMesh();
        GetOwner()->GetTransform()->SetScale(FTVector3(0.07f, 0.07f, 0.0f));
    }
}

void TileMapComponent::Update(float deltaTime)
{
    if (GetMeshGroup())
        MeshRendererComponent::Update(deltaTime);
}

void TileMapComponent::Render(FoxtrotRenderer* renderer)
{
    MeshRendererComponent::Render(renderer);
}

TileMapComponent::TileMapComponent(Actor* owner, int drawOrder, int UpdateOrder)
    : SpriteRendererComponent(owner, drawOrder)
    , mTileMap(nullptr)
    , mTileMapKey(VALUE_NOT_ASSIGNED)

{}

TileMapComponent::~TileMapComponent()
{
    ResourceManager::GetInstance()->RemoveLoadedMeshes(GetMeshKey());
}

void TileMapComponent::SaveProperties(std::ofstream& ofs)
{
    Component::SaveProperties(ofs);
    FileIOHelper::SaveUnsignedInt(ofs, ChunkKeys::TILEMAP_KEY, mTileMapKey);
}

void TileMapComponent::LoadProperties(std::ifstream& ifs)
{
    FileIOHelper::LoadUnsignedInt(ifs, mTileMapKey);
    Component::LoadProperties(ifs);
}

#ifdef FOXTROT_EDITOR

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include "imgui/FileDialog/ImGuiFileDialog.h"
#include "imgui/FileDialog/ImGuiFileDialogConfig.h"

void TileMapComponent::SaveProperties(nlohmann::ordered_json& out)
{
	Component::SaveProperties(out);
	FileIOHelper::AddScalarValue(out["MeshKey"], GetMeshKey());
	FileIOHelper::AddScalarValue(out["TextureKey"], GetTexKey());
}

void TileMapComponent::EditorUIUpdate()
{
    CHECK_RENDERER(GetRenderer());
    UpdateSprite();
    UpdateCSV();
    OnConfirmUpdate();
}

void TileMapComponent::OnConfirmUpdate()
{
    if (ImGui::Button("Update")) {
        if (!mTileMap->GetRelativePath().empty())
            this->InitializeTileMap();
        else
            printf("ERROR: TileMapComponent::ConfirmUpdate() -> .CSV path is null");
    }
}

void TileMapComponent::UpdateCSV() {
    
}
#endif