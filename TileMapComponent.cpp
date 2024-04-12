#include <queue>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "TileMapComponent.h"
#include "Actor.h"
#include "Math.h"
#include "Tile.h"
#include "Texture.h"
#include "Camera2D.h"
#include "Transform.h"
#include "CCore.h"

#ifdef _DEBUG
#include "EditorCamera2D.h"
#endif // _DEBUG

void TileMapComponent::InitializeTileMap(const std::wstring& fileName, int tileSizeX, int tileSizeY)
{
    ReadCSV(fileName, tileSizeX, tileSizeY);
    // Stores each tile with srcsRect values
    for (int y = 0; y < mTileCountY; y++)
        for (int x = 0; x < mTileCountX; x++)
            InitializeTile(&GetCurrentTileMap()[y][x]);
}

void TileMapComponent::ReadCSV(const std::wstring& fileName, int tileSizeX, int tileSizeY)
{
	std::ifstream myFile;
    std::queue<int> result;

	// Open an existing file 
    myFile.open(fileName, std::fstream::in);
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
    if(row != 0)
        column /= row;
    myFile.close();

    mTileCountX = column;
    mTileCountY = row;
	
    mCurrentTileMap = new Tile*[row];
    for (int r = 0; r < row; r++)
    {
        mCurrentTileMap[r] = new Tile[column];
        for (int c = 0; c < column; c++)
        {
            if (!result.empty())
            {
                mCurrentTileMap[r][c].SetWidth(tileSizeX);
                mCurrentTileMap[r][c].SetHeight(tileSizeY);
                mCurrentTileMap[r][c].SetTileNum(result.front());
                result.pop();
            }
        }
    }
}

void TileMapComponent::InitializeTile(Tile* tile)
{
    int tileSizeX = tile->GetWidth();
    int tileSizeY = tile->GetHeight();
    int columnCount = GetTexWidth() / tileSizeX;
    int srcColumn = 0; int srcRow = 0;
    srcColumn = tile->GetTileNum() % columnCount;
    srcRow = tile->GetTileNum() / columnCount;

    SDL_Rect srcRect = {};
    srcRect.x = srcColumn * tileSizeX;
    srcRect.y = srcRow * tileSizeY;
    srcRect.w = tileSizeX;
    srcRect.h = tileSizeY;
    tile->SetMapRect(srcRect);
}

void TileMapComponent::Render(SDL_Renderer* renderer)
{
    for (int y = 0; y < mTileCountY; y++)
    {
        for (int x = 0; x < mTileCountX; x++)
        {
            Tile* tile = &mCurrentTileMap[y][x];
            tile->SetIndexX(x);
            tile->SetIndexY(y);
            Vector2 pivotPos = GetOwner()->GetTransform()->GetWorldPosition();
            Vector2 pos =
                Vector2(pivotPos.x + tile->GetWidth() * tile->GetIndexX(), pivotPos.y + tile->GetHeight() * tile->GetIndexY());
            DrawIndividualTileOnPos(renderer, pos, tile);
        }
    }
}

void TileMapComponent::RenderEX(SDL_Renderer* renderer)
{
    for (int y = 0; y < mTileCountY; y++)
    {
        for (int x = 0; x < mTileCountX; x++)
        {
            Tile* tile = &mCurrentTileMap[y][x];
            tile->SetIndexX(x);
            tile->SetIndexY(y);
            Vector2 pivotPos = GetOwner()->GetTransform()->GetWorldPosition();
            Vector2 pos =
                Vector2(pivotPos.x + tile->GetWidth() * tile->GetIndexX(), pivotPos.y + tile->GetHeight() * tile->GetIndexY());
            DrawIndividualTileOnPosEX(renderer, pos, tile);
        }
    }
}

#ifdef _DEBUG
void TileMapComponent::DrawIndividualTileOnPos(
    SDL_Renderer* renderer, Vector2 worldPos, Tile* tile)
{
    SetScreenRectEditorView(worldPos, tile);
    if (GetTexture())
    {
        SDL_RenderCopy(
            renderer,
            GetTexture()->GetEditorTexture(),
            tile->GetMapRect(),
            tile->GetScreenRect()
        );
    }
    // Reset ScreenPos to re-calculate if tile's in render area
    tile->SetPosOnScreen(worldPos);
    if (Camera2D::GetInstance()->IsInRenderedArea(tile))
    {
        BlitToGameview(worldPos, tile);
    }
}

void TileMapComponent::BlitToGameview(Vector2 worldPos, Tile* tile)
{
    SDL_Renderer* gameviewRend = CCore::GetInstance()->GetGameRenderer();
    // 1) Get relative position from mLookAtPos
    SDL_Rect* renderArea = Camera2D::GetInstance()->GetRenderArea();
    Vector2 renderTopLeft = Vector2(renderArea->x, renderArea->y);
    Vector2 relativePos = worldPos - renderTopLeft;

    SDL_Rect gameViewRect = {};
    gameViewRect.x = static_cast<int>(relativePos.x - tile->GetWidth() * 0.5f);
    gameViewRect.y = static_cast<int>(relativePos.y - tile->GetHeight() * 0.5f);
    gameViewRect.w = tile->GetWidth();
    gameViewRect.h = tile->GetHeight();
    tile->SetScreenRect(gameViewRect);

    if (GetTexture()->GetGameviewTexture())
    {
        SDL_RenderCopy(
            gameviewRend,
            GetTexture()->GetGameviewTexture(),
            tile->GetMapRect(),
            tile->GetScreenRect()
        );
    }
}

void TileMapComponent::DrawIndividualTileOnPosEX(
    SDL_Renderer* renderer, Vector2 worldPos, Tile* tile)
{
    SetScreenRectEditorView(worldPos, tile);
    if (GetTexture())
    {
        SDL_RenderCopyEx(
            renderer,
            GetTexture()->GetEditorTexture(),
            tile->GetMapRect(),
            tile->GetScreenRect(),
            -Math::ToDegrees(GetOwner()->GetTransform()->GetRotation()),
            nullptr,
            SDL_FLIP_NONE
        );
    }
    tile->SetPosOnScreen(worldPos);
    if (Camera2D::GetInstance()->IsInRenderedArea(tile))
    {
        BlitToGameviewEx(worldPos, tile);
    }
}

void TileMapComponent::BlitToGameviewEx(Vector2 wPos, Tile* tile)
{
    SDL_Renderer* gameviewRend = CCore::GetInstance()->GetGameRenderer();
    // 1) Get relative position from mLookAtPos
    SDL_Rect* renderArea = Camera2D::GetInstance()->GetRenderArea();
    Vector2 renderCenter = Vector2(renderArea->x, renderArea->y);;
    Vector2 relativePos = wPos - renderCenter;

    SDL_Rect gameViewRect = {};
    gameViewRect.x = relativePos.x - tile->GetWidth() * 0.5f;
    gameViewRect.y = relativePos.y - tile->GetHeight() * 0.5f;
    gameViewRect.w = tile->GetWidth();
    gameViewRect.h = tile->GetHeight();
    tile->SetScreenRect(gameViewRect);

    if (GetTexture()->GetGameviewTexture())
    {
        SDL_RenderCopyEx(
            gameviewRend,
            GetTexture()->GetGameviewTexture(),
            tile->GetMapRect(),
            tile->GetScreenRect(),
            -Math::ToDegrees(GetOwner()->GetTransform()->GetRotation()),
            nullptr,
            SDL_FLIP_NONE
        );
    }
}

void TileMapComponent::SetScreenRectEditorView(Vector2 worldPos, Tile* tile)
{
    Vector2 editorPos = EditorCamera2D::GetInstance()->ConvertWorldPosToScreen(worldPos);
    SDL_Rect dstRect = {};
    dstRect.w = tile->GetWidth() * (1 - EditorCamera2D::GetInstance()->GetZoomValue());
    dstRect.h = tile->GetHeight() * (1 - EditorCamera2D::GetInstance()->GetZoomValue());
    dstRect.x = static_cast<int>(editorPos.x - (float)dstRect.w / 2);
    dstRect.y = static_cast<int>(editorPos.y - (float)dstRect.h / 2);
    tile->SetScreenRect(dstRect);
}

void TileMapComponent::SaveProperties(std::ofstream& ofs)
{
    SpriteComponent::SaveProperties(ofs);
}

void TileMapComponent::LoadProperties(std::ifstream& ifs)
{
    SpriteComponent::LoadProperties(ifs);
}

#else
void TileMapComponent::DrawIndividualTileOnPos(
    SDL_Renderer* renderer, Vector2 worldPos, Tile* tile)
{
    SetScreenRect(worldPos, tile);
    if (GetTexture())
    {
        SDL_RenderCopy(
            renderer,
            GetTexture()->GetGameviewTexture(),
            tile->GetMapRect(),
            tile->GetScreenRect()
        );
    }
    // Reset ScreenPos to re-calculate if tile's in render area
    tile->SetPosOnScreen(worldPos);
}

void TileMapComponent::DrawIndividualTileOnPosEX(
    SDL_Renderer* renderer, Vector2 worldPos, Tile* tile)
{
    SetScreenRect(worldPos, tile);
    if (GetTexture())
    {
        SDL_RenderCopyEx(
            renderer,
            GetTexture()->GetGameviewTexture(),
            tile->GetMapRect(),
            tile->GetScreenRect(),
            -Math::ToDegrees(GetOwner()->GetTransform()->GetRotation()),
            nullptr,
            SDL_FLIP_NONE
        );
    }
    tile->SetPosOnScreen(worldPos);
}

void TileMapComponent::SetScreenRect(Vector2 worldPos, Tile* tile)
{
    Vector2 screenPos = Camera2D::GetInstance()->ConvertWorldPosToScreen(worldPos);
    SDL_Rect dstRect = {};
    dstRect.w = tile->GetWidth();
    dstRect.h = tile->GetHeight();
    dstRect.x = static_cast<int>(screenPos.x - (float)dstRect.w / 2);
    dstRect.y = static_cast<int>(screenPos.y - (float)dstRect.h / 2);
    tile->SetScreenRect(dstRect);
}
#endif // _DEBUG

TileMapComponent::TileMapComponent(Actor* owner, int drawOrder, int UpdateOrder)
    : SpriteComponent(owner, drawOrder)
    , mTileCountX(0)
    , mTileCountY(0)
    , mCurrentTileMap(nullptr)

{}

TileMapComponent::~TileMapComponent()
{
    if (mCurrentTileMap != nullptr)
    {
        for (int row = 0; row < mTileCountY; row++)
        {
            delete[] mCurrentTileMap[row];
        }
        delete[] mCurrentTileMap;
    }
}