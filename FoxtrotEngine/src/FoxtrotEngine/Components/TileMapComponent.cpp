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
    
    // These Values will be read from files.
    mTileSizeOnScreen = FTVector2(1, 1);
    mTileSizeOnMap = FTVector2(32, 32);
    //SetTexKey("Edit here");
}

void TileMapComponent::InitializeTileMap() {
    if (!mCSVTileMapPath.empty())
    {
        SetTexture();
        assert(GetTexture());
        mMaxCountOnMapX = GetTexture()->GetTexWidth() / mTileSizeOnMap.x;
        mMaxCountOnMapY = GetTexture()->GetTexHeight() / mTileSizeOnMap.y;
        ReadCSV();
        assert(mMaxCountOnScreenX != 0 && mMaxCountOnScreenY != 0);
        ResourceManager::GetInstance()->LoadBasicMesh(
            mCSVTileMapPath,
            GeometryGenerator::MakeTileMapGrid(
                mCurrentTileMap,
                mMaxCountOnScreenX,
                mMaxCountOnScreenY
            )
        );
        SetMeshKey(mCSVTileMapPath);
        MeshRendererComponent::InitializeMesh();

        GetOwner()->GetTransform()->SetScale(FTVector3(0.07f, 0.07f, 0.0f));
    }
}

/*
    This defines...
    1) Max tile count on screen X
    2) Max tile count on screen Y
    3) Current Tile Map
*/ 
void TileMapComponent::ReadCSV()
{
    std::ifstream myFile;
    std::queue<int> result;

    // Open an existing file 
    myFile.open(mCSVTileMapPath, std::fstream::in);
    assert(myFile);
    std::string line;
    int val;
    int column = 0; int row = 0;
    while (std::getline(myFile, line))
    {
        std::stringstream ss(line);
        // Create a stringstream of the current line
        // Extract each integer
        while (ss >> val)
        {
            result.push(val);
            if (ss.peek() == ',') ss.ignore();
            ++column;
        }
        ++row;
    }
    if (row != 0)
        column /= row;
    myFile.close();

    mMaxCountOnScreenX = column;
    mMaxCountOnScreenY = row;

    mCurrentTileMap = new Tile * [row];
    for (int r = 0; r < row; ++r)
    {
        mCurrentTileMap[r] = new Tile[column];
        for (int c = 0; c < column; ++c)
        {
            if (!result.empty())
            {
                InitializeTile(&mCurrentTileMap[r][c], c, r, result.front());
                result.pop();
            }
        }
    }
}

void TileMapComponent::InitializeTile(Tile* tile, UINT column, UINT row, UINT tileNum)
{
    FTRect* rectOnMap  = tile->GetRectOnMap();
    // Individual Tile size on tilemap
    float tileWidth  = static_cast<float>(mTileSizeOnMap.x) / static_cast<float>(this->GetTexWidth());
    float tileHeight = static_cast<float>(mTileSizeOnMap.y) / static_cast<float>(this->GetTexHeight());

    int tileIndexX = tileNum % mMaxCountOnMapX;
    int tileIndexY = tileNum / mMaxCountOnMapX;
    rectOnMap->Set(tileWidth * tileIndexX, tileHeight * tileIndexY, tileWidth, tileHeight);

    FTRect* rectOnScreen = tile->GetRectOnScreen();
    rectOnScreen->Set(column, row, mTileSizeOnScreen.x, mTileSizeOnScreen.y);
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

//void TileMapComponent::Render(FoxtrotRenderer* renderer)
//{
//    MeshRendererComponent::Render(renderer);
//}

//
//void TileMapComponent::RenderEX(FoxtrotRenderer* renderer)
//{
//    for (int y = 0; y < mTileCountY; y++)
//    {
//        for (int x = 0; x < mTileCountX; x++)
//        {
//            Tile* tile = &mCurrentTileMap[y][x];
//            tile->SetIndexX(x);
//            tile->SetIndexY(y);
//            FTVector2 pivotPos = GetOwner()->GetTransform()->GetWorldPosition();
//            FTVector2 pos =
//                FTVector2(pivotPos.x + tile->GetWidth() * tile->GetIndexX(), pivotPos.y + tile->GetHeight() * tile->GetIndexY());
//            /*
//            * ALTERNATIVE FOR
//            DrawIndividualTileOnPosEX(renderer, pos, tile);
//            */
//        }
//    }
//}

TileMapComponent::TileMapComponent(Actor* owner, int drawOrder, int UpdateOrder)
    : SpriteRendererComponent(owner, drawOrder)
    , mCurrentTileMap(nullptr)

{}

TileMapComponent::~TileMapComponent()
{
    ResourceManager::GetInstance()->RemoveLoadedMeshes(GetMeshKey());
    if (mCurrentTileMap != nullptr)
    {
        for (int row = 0; row < mMaxCountOnScreenY; row++)
        {
            delete[] mCurrentTileMap[row];
        }
        delete[] mCurrentTileMap;
    }
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
        if (!mCSVTileMapPath.empty())
            this->InitializeTileMap();
        else
            printf("ERROR: TileMapComponent::ConfirmUpdate() -> .CSV path is null");
    }
}

void TileMapComponent::UpdateCSV() {
    std::string currentCSV = "No .CSV has been assigned";
    if (!mCSVTileMapPath.empty())
        currentCSV = "Current .CSV : \n" + mCSVTileMapPath;
    ImGui::Text(currentCSV.c_str());

    if (ImGui::Button("Select CSV")) {
        IGFD::FileDialogConfig config;
        config.path = ".";
        config.countSelectionMax = 1;
        ImGuiFileDialog::Instance()->OpenDialog("SelectCSV", "Select CSV", ".csv", config);
        ImGui::OpenPopup("Select .CSV");
    }

    if (ImGuiFileDialog::Instance()->Display("SelectCSV"))
    {
        if (ImGuiFileDialog::Instance()->IsOk())
        {
            mCSVTileMapPath = ImGuiFileDialog::Instance()->GetFilePathName();
        }
        ImGuiFileDialog::Instance()->Close();
    }
}
#endif // _DEBUG

void TileMapComponent::SaveProperties(nlohmann::ordered_json& out)
{
    Component::SaveProperties(out);
    FileIOHelper::AddScalarValue(out["MeshKey"], GetMeshKey());
    FileIOHelper::AddScalarValue(out["TextureKey"], GetTexKey());
    FileIOHelper::AddVector2(out["TileSizeOnScreen"], mTileSizeOnScreen);
    FileIOHelper::AddVector2(out["TileSizeOnMap"], mTileSizeOnMap);
}

void TileMapComponent::LoadProperties(std::ifstream& ifs)
{
    SpriteRendererComponent::LoadProperties(ifs);
}