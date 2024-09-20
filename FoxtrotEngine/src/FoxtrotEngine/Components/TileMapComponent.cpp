#include "FoxtrotEngine/Components/TileMapComponent.h"

#include <queue>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "FoxtrotEngine/Actors/Actor.h"
#include "FoxtrotEngine/Math/FTMath.h"
#include "FoxtrotEngine/ResourceSystem/Tile.h"
#include "FoxtrotEngine/ResourceSystem/FTTexture.h"

#include "FoxtrotEngine/Actors/Transform.h"
#include "FoxtrotEngine/Core/FTCore.h"
#include "FoxtrotEngine/ResourceSystem/Mesh.h"
#include "FoxtrotEngine/ResourceSystem/FTBasicMeshGroup.h"
#include "FoxtrotEngine/ResourceSystem/GeometryGenerator.h"
#include "FoxtrotEngine/Renderer/Camera.h"
#include "FoxtrotEngine/Renderer/FoxtrotRenderer.h"
#include "FoxtrotEngine/Managers/ResourceManager.h"
#include "FoxtrotEngine/Renderer/FTRect.h"
#include "FoxtrotEngine/FileSystem/ChunkLoader.h"

#define DEFAULT_TILE_POS 0

void TileMapComponent::Initialize(FTCore* coreInstance){
    MeshRendererComponent::Initialize(coreInstance);
    //SetTexKey("Edit here");
}

void TileMapComponent::InitializeTileMap() {
    assert(mTileMapKey.empty());
    assert(GetTexKey().empty());
    assert(GetMeshKey().empty());
    mTileMap = ResourceManager::GetInstance()->GetLoadedTileMap(mTileMapKey);
    if (mTileMap)
    {
        SetTexture();
        mTileMap->ReadCSV();
        ResourceManager::GetInstance()->LoadBasicMesh(
            GetMeshKey(),
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

{}

TileMapComponent::~TileMapComponent()
{
    ResourceManager::GetInstance()->RemoveLoadedMeshes(GetMeshKey());
}

#ifdef _DEBUG
#include "FoxtrotEditor/CommandHistory.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include "imgui/FileDialog/ImGuiFileDialog.h"
#include "imgui/FileDialog/ImGuiFileDialogConfig.h"

void TileMapComponent::EditorUIUpdate()
{
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
#endif // _DEBUG

void TileMapComponent::SaveProperties(nlohmann::ordered_json& out)
{
    Component::SaveProperties(out);
    FileIOHelper::AddScalarValue(out["MeshKey"], GetMeshKey());
    FileIOHelper::AddScalarValue(out["TextureKey"], GetTexKey());
}

void TileMapComponent::LoadProperties(std::ifstream& ifs)
{
    SpriteRendererComponent::LoadProperties(ifs);
}