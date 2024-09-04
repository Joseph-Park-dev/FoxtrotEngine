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

#ifdef _DEBUG

#endif // _DEBUG

#define DEFAULT_TILE_POS 0

void TileMapComponent::Initialize(FTCore* coreInstance){
    MeshRendererComponent::Initialize(coreInstance);
    mCSVTileMapPath     = "D:/[2024_01]/FoxtrotEngine_DirectX/assets/MapLayer1.csv";
    mTileSizeOnScreenX  = 1;
    mTileSizeOnScreenY  = 1;
    mTileSizeOnMapX     = 32;
    mTileSizeOnMapY     = 32;

    mMaxCountOnMapX     = 8;
    mMaxCountOnMapY     = 32;
    
    SetTexture("TestTileTex");
    InitializeTileMap();
    ResourceManager::GetInstance()->LoadBasicMesh(
        MAPKEY_SQUARE_GRID,
        GeometryGenerator::MakeTileMapGrid(
            mCurrentTileMap,
            mMaxCountOnScreenX,
            mMaxCountOnScreenY
        )
    );
    SetMeshKey(MAPKEY_SQUARE_GRID);
    MeshRendererComponent::InitializeMesh();

    GetOwner()->GetTransform()->SetScale(FTVector3(0.07f, 0.07f, 0.0f));
}

void TileMapComponent::InitializeTileMap() {
    if (!mCSVTileMapPath.empty())
        ReadCSV();
}

void TileMapComponent::ReadCSV()
{
    std::ifstream myFile;
    std::queue<int> result;

    // Open an existing file 
    myFile.open(mCSVTileMapPath, std::fstream::in);
    assert(myFile);
    assert(mMaxCountOnScreenX != 0 && mMaxCountOnScreenY != 0);
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
    float tileWidth  = static_cast<float>(mTileSizeOnMapX) / static_cast<float>(this->GetTexWidth());
    float tileHeight = static_cast<float>(mTileSizeOnMapY) / static_cast<float>(this->GetTexHeight());

    int tileIndexX = tileNum % mMaxCountOnMapX;
    int tileIndexY = tileNum / mMaxCountOnMapX;
    rectOnMap->Set(tileWidth * tileIndexX, tileHeight * tileIndexY, tileWidth, tileHeight);

    FTRect* rectOnScreen = tile->GetRectOnScreen();
    rectOnScreen->Set(column, row, mTileSizeOnScreenX, mTileSizeOnScreenY);
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
//void TileMapComponent::SetScreenRectEditorView(FTVector2 worldPos, Tile* tile)
//{
//    FTVector2 editorPos = EditorCamera2D::GetInstance()->ConvertWorldPosToScreen(worldPos);
//    Bounds dstRect = {};
//    dstRect.w = tile->GetWidth() * (1 - EditorCamera2D::GetInstance()->GetZoomValue());
//    dstRect.h = tile->GetHeight() * (1 - EditorCamera2D::GetInstance()->GetZoomValue());
//    dstRect.x = static_cast<int>(editorPos.x - (float)dstRect.w / 2);
//    dstRect.y = static_cast<int>(editorPos.y - (float)dstRect.h / 2);
//    tile->SetScreenRect(dstRect);
//}

void TileMapComponent::SaveProperties(nlohmann::ordered_json& out)
{
    SpriteRendererComponent::SaveProperties(out);
}

void TileMapComponent::LoadProperties(std::ifstream& ifs)
{
    SpriteRendererComponent::LoadProperties(ifs);
}
#endif // _DEBUG

//#ifdef _DEBUG
//void TileMapComponent::DrawIndividualTileOnPos(
//    FoxtrotRenderer* renderer, FTVector2 worldPos, Tile* tile)
//{
//    SetScreenRectEditorView(worldPos, tile);
//    if (GetTexture())
//    {
//        SDL_RenderCopy(
//            renderer,
//            GetTexture()->GetEditorTexture(),
//            tile->GetMapRect(),
//            tile->GetScreenRect()
//        );
//    }
//    // Reset ScreenPos to re-calculate if tile's in render area
//    tile->SetPosOnScreen(worldPos);
//    if (Camera2D::GetInstance()->IsInRenderedArea(tile))
//    {
//        BlitToGameview(worldPos, tile);
//    }
//}
//
//void TileMapComponent::BlitToGameview(FTVector2 worldPos, Tile* tile)
//{
//    FoxtrotRenderer* gameviewRend = FTCore::GetInstance()->GetGameRenderer();
//    // 1) Get relative position from mLookAtPos
//    SDL_Rect* renderArea = Camera2D::GetInstance()->GetRenderArea();
//    FTVector2 renderTopLeft = FTVector2(renderArea->x, renderArea->y);
//    FTVector2 relativePos = worldPos - renderTopLeft;
//
//    SDL_Rect gameViewRect = {};
//    gameViewRect.x = static_cast<int>(relativePos.x - tile->GetWidth() * 0.5f);
//    gameViewRect.y = static_cast<int>(relativePos.y - tile->GetHeight() * 0.5f);
//    gameViewRect.w = tile->GetWidth();
//    gameViewRect.h = tile->GetHeight();
//    tile->SetScreenRect(gameViewRect);
//
//    if (GetTexture()->GetGameviewTexture())
//    {
//        SDL_RenderCopy(
//            gameviewRend,
//            GetTexture()->GetGameviewTexture(),
//            tile->GetMapRect(),
//            tile->GetScreenRect()
//        );
//    }
//}
//
//void TileMapComponent::DrawIndividualTileOnPosEX(
//    FoxtrotRenderer* renderer, FTVector2 worldPos, Tile* tile)
//{
//    SetScreenRectEditorView(worldPos, tile);
//    if (GetTexture())
//    {
//        SDL_RenderCopyEx(
//            renderer,
//            GetTexture()->GetEditorTexture(),
//            tile->GetMapRect(),
//            tile->GetScreenRect(),
//            -Math::ToDegrees(GetOwner()->GetTransform()->GetRotation()),
//            nullptr,
//            SDL_FLIP_NONE
//        );
//    }
//    tile->SetPosOnScreen(worldPos);
//    if (Camera2D::GetInstance()->IsInRenderedArea(tile))
//    {
//        BlitToGameviewEx(worldPos, tile);
//    }
//}
//
//void TileMapComponent::BlitToGameviewEx(FTVector2 wPos, Tile* tile)
//{
//    FoxtrotRenderer* gameviewRend = FTCore::GetInstance()->GetGameRenderer();
//    // 1) Get relative position from mLookAtPos
//    SDL_Rect* renderArea = Camera2D::GetInstance()->GetRenderArea();
//    FTVector2 renderCenter = FTVector2(renderArea->x, renderArea->y);;
//    FTVector2 relativePos = wPos - renderCenter;
//
//    SDL_Rect gameViewRect = {};
//    gameViewRect.x = relativePos.x - tile->GetWidth() * 0.5f;
//    gameViewRect.y = relativePos.y - tile->GetHeight() * 0.5f;
//    gameViewRect.w = tile->GetWidth();
//    gameViewRect.h = tile->GetHeight();
//    tile->SetScreenRect(gameViewRect);
//
//    if (GetTexture()->GetGameviewTexture())
//    {
//        SDL_RenderCopyEx(
//            gameviewRend,
//            GetTexture()->GetGameviewTexture(),
//            tile->GetMapRect(),
//            tile->GetScreenRect(),
//            -Math::ToDegrees(GetOwner()->GetTransform()->GetRotation()),
//            nullptr,
//            SDL_FLIP_NONE
//        );
//    }
//}

//#else
//void TileMapComponent::DrawIndividualTileOnPos(
//    FoxtrotRenderer* renderer, FTVector2 worldPos, Tile* tile)
//{
//    SetScreenRect(worldPos, tile);
//    if (GetTexture())
//    {
//        SDL_RenderCopy(
//            renderer,
//            GetTexture()->GetGameviewTexture(),
//            tile->GetMapRect(),
//            tile->GetScreenRect()
//        );
//    }
//    // Reset ScreenPos to re-calculate if tile's in render area
//    tile->SetPosOnScreen(worldPos);
//}
//
//void TileMapComponent::DrawIndividualTileOnPosEX(
//    FoxtrotRenderer* renderer, FTVector2 worldPos, Tile* tile)
//{
//    SetScreenRect(worldPos, tile);
//    if (GetTexture())
//    {
//        SDL_RenderCopyEx(
//            renderer,
//            GetTexture()->GetGameviewTexture(),
//            tile->GetMapRect(),
//            tile->GetScreenRect(),
//            -Math::ToDegrees(GetOwner()->GetTransform()->GetRotation()),
//            nullptr,
//            SDL_FLIP_NONE
//        );
//    }
//    tile->SetPosOnScreen(worldPos);
//}
//
//void TileMapComponent::SetScreenRect(FTVector2 worldPos, Tile* tile)
//{
//    FTVector2 screenPos = Camera2D::GetInstance()->ConvertWorldPosToScreen(worldPos);
//    SDL_Rect dstRect = {};
//    dstRect.w = tile->GetWidth();
//    dstRect.h = tile->GetHeight();
//    dstRect.x = static_cast<int>(screenPos.x - (float)dstRect.w / 2);
//    dstRect.y = static_cast<int>(screenPos.y - (float)dstRect.h / 2);
//    tile->SetScreenRect(dstRect);
//}
//#endif // _DEBUG