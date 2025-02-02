﻿// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "ResourceSystem/GeometryGenerator.h"

#include <directxtk/SimpleMath.h>
#include <vector>

#include "Core/TemplateFunctions.h"
#include "Math/FTMath.h"
#include "ResourceSystem/Vertex.h"
#include "ResourceSystem/MeshData.h"
#include "ResourceSystem/FTTexture.h"
#include "ResourceSystem/FTTileMap.h"
#include "ResourceSystem/Tile.h"
#include "ResourceSystem/TextAttribute.h"

//#include "ModelLoader.h"
//using namespace DirectX;
//using namespace DirectX::SimpleMath;
//using D3DVec2 = DirectX::SimpleMath::Vector2;
//using D3DVec3 = DirectX::SimpleMath::Vector3;

using Vector3 = DirectX::SimpleMath::Vector3;
using Vector2 = DirectX::SimpleMath::Vector2;

MeshData GeometryGenerator::MakeSquare() {
    std::vector<Vector3> positions;
    std::vector<Vector3> colors;
    std::vector<Vector3> normals;
    std::vector<Vector2> texcoords;

    positions.reserve(4);
    colors.reserve(4);
    normals.reserve(4);
    texcoords.reserve(4);

    positions.push_back(Vector3(-0.5f, 0.5f, 0.0f));
    positions.push_back(Vector3(0.5f, 0.5f, 0.0f));
    positions.push_back(Vector3(0.5f, -0.5f, 0.0f));
    positions.push_back(Vector3(-0.5f, -0.5f, 0.0f));

    colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
    colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
    colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
    colors.push_back(Vector3(0.0f, 0.0f, 1.0f));

    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));

    texcoords.push_back(Vector2(0.0f, 0.0f));
    texcoords.push_back(Vector2(1.0f, 0.0f));
    texcoords.push_back(Vector2(1.0f, 1.0f));
    texcoords.push_back(Vector2(0.0f, 1.0f));

    MeshData meshData;
    for (size_t i = 0; i < positions.size(); i++) {
        Vertex v;
        v.position = positions[i];
        v.color = colors[i];
        v.normal = normals[i];
        v.texcoord = texcoords[i];
        meshData.vertices.push_back(v);
    }
    meshData.indices = {
        0, 1, 2, 0, 2, 3, // 앞면
    };

    return meshData;
}

MeshData GeometryGenerator::MakeSquare(float scale) {
    std::vector<Vector3> positions;
    std::vector<Vector3> colors;
    std::vector<Vector3> normals;
    std::vector<Vector2> texcoords;

    positions.reserve(4);
    colors.reserve(4);
    normals.reserve(4);
    texcoords.reserve(4);

    positions.push_back(Vector3(-0.5f, 0.5f, 0.0f) * scale);
    positions.push_back(Vector3(0.5f, 0.5f, 0.0f) * scale);
    positions.push_back(Vector3(0.5f, -0.5f, 0.0f) * scale);
    positions.push_back(Vector3(-0.5f, -0.5f, 0.0f) * scale);

    colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
    colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
    colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
    colors.push_back(Vector3(0.0f, 0.0f, 1.0f));

    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));

    texcoords.push_back(Vector2(0.0f, 0.0f));
    texcoords.push_back(Vector2(1.0f, 0.0f));
    texcoords.push_back(Vector2(1.0f, 1.0f));
    texcoords.push_back(Vector2(0.0f, 1.0f));

    MeshData meshData;
    for (size_t i = 0; i < positions.size(); i++) {
        Vertex v;
        v.position = positions[i];
        v.color = colors[i];
        v.normal = normals[i];
        v.texcoord = texcoords[i];
        meshData.vertices.push_back(v);
    }
    meshData.indices = {
        0, 1, 2, 0, 2, 3, // 앞면
    };

    return meshData;
}

MeshData GeometryGenerator::MakeSquare(FTVector3 color)
{
    std::vector<Vector3> positions;
    std::vector<Vector3> colors;
    std::vector<Vector3> normals;
    std::vector<Vector2> texcoords;

    positions.reserve(4);
    colors.reserve(4);
    normals.reserve(4);
    texcoords.reserve(4);

    positions.push_back(Vector3(-0.5f, 0.5f, 0.0f));
    positions.push_back(Vector3(0.5f, 0.5f, 0.0f));
    positions.push_back(Vector3(0.5f, -0.5f, 0.0f));
    positions.push_back(Vector3(-0.5f, -0.5f, 0.0f));

    colors.push_back(Vector3(color.GetDXVec3()));
    colors.push_back(Vector3(color.GetDXVec3()));
    colors.push_back(Vector3(color.GetDXVec3()));
    colors.push_back(Vector3(color.GetDXVec3()));

    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));

    texcoords.push_back(Vector2(0.0f, 0.0f));
    texcoords.push_back(Vector2(1.0f, 0.0f));
    texcoords.push_back(Vector2(1.0f, 1.0f));
    texcoords.push_back(Vector2(0.0f, 1.0f));

    MeshData meshData;
    for (size_t i = 0; i < positions.size(); i++) {
        Vertex v;
        v.position = positions[i];
        v.color = colors[i];
        v.normal = normals[i];
        v.texcoord = texcoords[i];
        meshData.vertices.push_back(v);
    }
    meshData.indices = {
        0, 1, 2, 0, 2, 3, // 앞면
    };

    return meshData;
}

MeshData GeometryGenerator::MakeSquare(float scale, FTVector3 color)
{
    std::vector<Vector3> positions;
    std::vector<Vector3> colors;
    std::vector<Vector3> normals;
    std::vector<Vector2> texcoords;

    positions.reserve(4);
    colors.reserve(4);
    normals.reserve(4);
    texcoords.reserve(4);

    positions.push_back(Vector3(-0.5f, 0.5f, 0.0f) * scale);
    positions.push_back(Vector3(0.5f, 0.5f, 0.0f) * scale);
    positions.push_back(Vector3(0.5f, -0.5f, 0.0f) * scale);
    positions.push_back(Vector3(-0.5f, -0.5f, 0.0f) * scale);

    colors.push_back(color.GetDXVec3());
    colors.push_back(color.GetDXVec3());
    colors.push_back(color.GetDXVec3());
    colors.push_back(color.GetDXVec3());

    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));

    texcoords.push_back(Vector2(0.0f, 0.0f));
    texcoords.push_back(Vector2(1.0f, 0.0f));
    texcoords.push_back(Vector2(1.0f, 1.0f));
    texcoords.push_back(Vector2(0.0f, 1.0f));

    MeshData meshData;
    for (size_t i = 0; i < positions.size(); i++) {
        Vertex v;
        v.position = positions[i];
        v.color = colors[i];
        v.normal = normals[i];
        v.texcoord = texcoords[i];
        meshData.vertices.push_back(v);
    }
    meshData.indices = {
        0, 1, 2, 0, 2, 3, // 앞면
    };

    return meshData;
}

MeshData GeometryGenerator::MakeSquareGrid(
    const int numSlices,
    const int numStacks,
    const float scale,
    const Vector2 texScale) {
    MeshData meshData;

    float dx = 2.0f / numSlices;
    float dy = 2.0f / numStacks;

    float y = 1.0f;
    for (int j = 0; j < numStacks + 1; j++) {
        float x = -1.0f;
        for (int i = 0; i < numSlices + 1; i++) {
            Vertex v;
            v.position = Vector3(x, y, 0.0f) * scale;
            v.texcoord = Vector2(1.0f) - (Vector2(-x + 1.0f, y + 1.0f) * 0.5f * texScale);

            printf("%f    %f\n", v.texcoord.x, v.texcoord.y);

            meshData.vertices.push_back(v);

            x += dx;
        }
        y -= dy;
    }

    for (int j = 0; j < numStacks; j++) {
        for (int i = 0; i < numSlices; i++) {
            meshData.indices.push_back((numSlices + 1) * j + i);
            meshData.indices.push_back((numSlices + 1) * j + i + 1);
            meshData.indices.push_back((numSlices + 1) * (j + 1) + i);
            meshData.indices.push_back((numSlices + 1) * (j + 1) + i);
            meshData.indices.push_back((numSlices + 1) * j + i + 1);
            meshData.indices.push_back((numSlices + 1) * (j + 1) + i + 1);
        }
    }
    return meshData;
}

std::vector<MeshData> GeometryGenerator::MakeTileMapGrid(FTTileMap* tileMap)
{
    std::vector<MeshData> tileMapMeshes;
    LogInt("X : ", tileMap->GetMaxCountOnScreenX());
    LogInt("Y : ", tileMap->GetMaxCountOnScreenY());
    for (size_t r = 0; r < tileMap->GetMaxCountOnScreenY(); ++r) {
        for (size_t c = 0; c < tileMap->GetMaxCountOnScreenX(); ++c) {
            Tile& tile = tileMap->GetTile(r,c);
            MeshData tileMesh = MakeTile(tile);
            tileMapMeshes.push_back(tileMesh);
        }
    }
    return tileMapMeshes;
}

MeshData GeometryGenerator::MakeTile(Tile& tile)
{
    std::vector<Vector3> positions;
    std::vector<Vector3> colors;
    std::vector<Vector3> normals;
    std::vector<Vector2> texcoords; // 텍스춰 좌표

    FTRect& rectOnScreen = tile.GetRectOnScreen();

    const FTVector2& tileMin = rectOnScreen.GetMin();
    const FTVector2& tileMax = rectOnScreen.GetMax();
    const float tileWidth = rectOnScreen.GetSize().x;
    const float tileHeight = rectOnScreen.GetSize().y;

    // 앞면
    positions.push_back(Vector3(tileMin.x,              -tileMin.y,              0.0f));
    positions.push_back(Vector3(tileMin.x + tileWidth,  -tileMin.y,              0.0f));
    positions.push_back(Vector3(tileMin.x + tileWidth,  -tileMin.y - tileHeight, 0.0f));
    positions.push_back(Vector3(tileMin.x,              -tileMin.y - tileHeight, 0.0f));
    colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
    colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
    colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
    colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));

    // Texture Coordinates
    FTRect& rectOnMap = tile.GetRectOnMap();
    const FTVector2& mapMin = rectOnMap.GetMin();
    const float widthInMap = rectOnMap.GetSize().x;
    const float heightInMap = rectOnMap.GetSize().y;

    texcoords.push_back(Vector2(mapMin.x,               mapMin.y));
    texcoords.push_back(Vector2(mapMin.x + widthInMap,  mapMin.y));
    texcoords.push_back(Vector2(mapMin.x + widthInMap,  mapMin.y + heightInMap));
    texcoords.push_back(Vector2(mapMin.x,               mapMin.y + heightInMap));

    MeshData meshData;
    for (size_t i = 0; i < positions.size(); i++) {
        Vertex v;
        v.position = positions[i];
        v.color = colors[i];
        v.normal = normals[i];
        v.texcoord = texcoords[i];
        meshData.vertices.push_back(v);
    }
    meshData.indices = {
        0, 1, 2, 0, 2, 3, // 앞면
    };

    return meshData;
}

void GeometryGenerator::MakeSpriteAnimation(std::vector<MeshData>& animMeshes, Tile* tileMap, size_t columnCount, size_t rowCount)
{
    for (size_t r = 0; r < rowCount; ++r) {
        for (size_t c = 0; c < columnCount; ++c) {
            size_t idx = columnCount * r + c;
            MeshData animFrame = MakeAnimationFrame(tileMap[columnCount * r + c]);
            animMeshes.push_back(animFrame);
        }
    }
}

MeshData GeometryGenerator::MakeAnimationFrame(Tile tile)
{
    std::vector<Vector3> positions;
    std::vector<Vector3> colors;
    std::vector<Vector3> normals;
    std::vector<Vector2> texcoords; // 텍스춰 좌표

    // 앞면
    positions.push_back(Vector3(-1.0f, 1.0f, 0.0f));
    positions.push_back(Vector3(1.0f, 1.0f, 0.0f));
    positions.push_back(Vector3(1.0f, -1.0f, 0.0f));
    positions.push_back(Vector3(-1.0f, -1.0f, 0.0f));
    colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
    colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
    colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
    colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));

    // Texture Coordinates (Direct3D 9)
    // https://learn.microsoft.com/en-us/windows/win32/direct3d9/texture-coordinates
    FTRect& rectOnMap = tile.GetRectOnMap();
    const FTVector2& mapMin = rectOnMap.GetMin();
    const float widthInMap = rectOnMap.GetSize().x;
    const float heightInMap = rectOnMap.GetSize().y;

    texcoords.push_back(Vector2(mapMin.x, mapMin.y));
    texcoords.push_back(Vector2(mapMin.x + widthInMap, mapMin.y));
    texcoords.push_back(Vector2(mapMin.x + widthInMap, mapMin.y + heightInMap));
    texcoords.push_back(Vector2(mapMin.x, mapMin.y + heightInMap));

    MeshData meshData;
    for (size_t i = 0; i < positions.size(); i++) {
        Vertex v;
        v.position = positions[i];
        v.color = colors[i];
        v.normal = normals[i];
        v.texcoord = texcoords[i];
        meshData.vertices.push_back(v);
    }
    meshData.indices = {
        0, 1, 2, 0, 2, 3, // 앞면
    };

    return meshData;
}

void GeometryGenerator::MakeSpriteTextGrid(std::vector<MeshData>& textMeshes, Tile* tileMap, size_t length, TextAttribute* attribute)
{
    if (0 < attribute->MaxChars)
    {
        size_t maxRow = length / attribute->MaxChars < 1 ? 1 : length / attribute->MaxChars;
        for (size_t i = 0; i <= maxRow; ++i)
        {
            for (size_t j = 0; j < attribute->MaxChars; ++j)
            {
                size_t idx = attribute->MaxChars * i + j;
                MeshData spriteText = MakeSpriteText(tileMap[idx], attribute, j, i);
                textMeshes.push_back(spriteText);
            }
        }
    }
}

MeshData GeometryGenerator::MakeSpriteText(Tile tile, TextAttribute* attrib, size_t col, size_t row)
{
    std::vector<Vector3> positions;
    std::vector<Vector3> colors;
    std::vector<Vector3> normals;
    std::vector<Vector2> texcoords; // 텍스춰 좌표

    FTRect& rectOnScreen = tile.GetRectOnScreen();

    const float tileWidth = rectOnScreen.GetSize().x * attrib->Scale;
    const float tileHeight = rectOnScreen.GetSize().y * attrib->Scale;
    FTVector2 tileMin = FTVector2(tileWidth * col, tileHeight * row) * FTVector2(attrib->CharSpacing, 1.f);

    // 앞면
    positions.push_back(Vector3(tileMin.x,             -tileMin.y,              0.0f));
    positions.push_back(Vector3(tileMin.x + tileWidth, -tileMin.y,              0.0f));
    positions.push_back(Vector3(tileMin.x + tileWidth, -tileMin.y - tileHeight, 0.0f));
    positions.push_back(Vector3(tileMin.x,             -tileMin.y - tileHeight, 0.0f));
    colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
    colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
    colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
    colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));

    // Texture Coordinates (Direct3D 9)
    // https://learn.microsoft.com/en-us/windows/win32/direct3d9/texture-coordinates
    FTRect& rectOnMap = tile.GetRectOnMap();
    const FTVector2& mapMin = rectOnMap.GetMin();
    const float widthInMap = rectOnMap.GetSize().x;
    const float heightInMap = rectOnMap.GetSize().y;

    texcoords.push_back(Vector2(mapMin.x, mapMin.y));
    texcoords.push_back(Vector2(mapMin.x + widthInMap, mapMin.y));
    texcoords.push_back(Vector2(mapMin.x + widthInMap, mapMin.y + heightInMap));
    texcoords.push_back(Vector2(mapMin.x, mapMin.y + heightInMap));

    MeshData meshData;
    for (size_t i = 0; i < positions.size(); i++) {
        Vertex v;
        v.position = positions[i];
        v.color = colors[i];
        v.normal = normals[i];
        v.texcoord = texcoords[i];
        meshData.vertices.push_back(v);
    }
    meshData.indices = {
        0, 1, 2, 0, 2, 3, // 앞면
    };

    return meshData;
}

MeshData GeometryGenerator::MakeBox() {
    std::vector<Vector3> positions;
    std::vector<Vector3> colors;
    std::vector<Vector3> normals;
    std::vector<Vector2> texcoords; // 텍스춰 좌표

    const float scale = 1.0f;

    // 윗면
    positions.push_back(Vector3(-1.0f, 1.0f, -1.0f) * scale);
    positions.push_back(Vector3(-1.0f, 1.0f, 1.0f) * scale);
    positions.push_back(Vector3(1.0f, 1.0f, 1.0f) * scale);
    positions.push_back(Vector3(1.0f, 1.0f, -1.0f) * scale);
    colors.push_back(Vector3(1.0f, 0.0f, 0.0f));
    colors.push_back(Vector3(1.0f, 0.0f, 0.0f));
    colors.push_back(Vector3(1.0f, 0.0f, 0.0f));
    colors.push_back(Vector3(1.0f, 0.0f, 0.0f));
    normals.push_back(Vector3(0.0f, 1.0f, 0.0f));
    normals.push_back(Vector3(0.0f, 1.0f, 0.0f));
    normals.push_back(Vector3(0.0f, 1.0f, 0.0f));
    normals.push_back(Vector3(0.0f, 1.0f, 0.0f));
    texcoords.push_back(Vector2(0.0f, 0.0f));
    texcoords.push_back(Vector2(1.0f, 0.0f));
    texcoords.push_back(Vector2(1.0f, 1.0f));
    texcoords.push_back(Vector2(0.0f, 1.0f));

    // 아랫면
    positions.push_back(Vector3(-1.0f, -1.0f, -1.0f) * scale);
    positions.push_back(Vector3(1.0f, -1.0f, -1.0f) * scale);
    positions.push_back(Vector3(1.0f, -1.0f, 1.0f) * scale);
    positions.push_back(Vector3(-1.0f, -1.0f, 1.0f) * scale);
    colors.push_back(Vector3(0.0f, 1.0f, 0.0f));
    colors.push_back(Vector3(0.0f, 1.0f, 0.0f));
    colors.push_back(Vector3(0.0f, 1.0f, 0.0f));
    colors.push_back(Vector3(0.0f, 1.0f, 0.0f));
    normals.push_back(Vector3(0.0f, -1.0f, 0.0f));
    normals.push_back(Vector3(0.0f, -1.0f, 0.0f));
    normals.push_back(Vector3(0.0f, -1.0f, 0.0f));
    normals.push_back(Vector3(0.0f, -1.0f, 0.0f));
    texcoords.push_back(Vector2(0.0f, 0.0f));
    texcoords.push_back(Vector2(1.0f, 0.0f));
    texcoords.push_back(Vector2(1.0f, 1.0f));
    texcoords.push_back(Vector2(0.0f, 1.0f));

    // 앞면
    positions.push_back(Vector3(-1.0f, -1.0f, -1.0f) * scale);
    positions.push_back(Vector3(-1.0f, 1.0f, -1.0f) * scale);
    positions.push_back(Vector3(1.0f, 1.0f, -1.0f) * scale);
    positions.push_back(Vector3(1.0f, -1.0f, -1.0f) * scale);
    colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
    colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
    colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
    colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
    texcoords.push_back(Vector2(0.0f, 0.0f));
    texcoords.push_back(Vector2(1.0f, 0.0f));
    texcoords.push_back(Vector2(1.0f, 1.0f));
    texcoords.push_back(Vector2(0.0f, 1.0f));

    // 뒷면
    positions.push_back(Vector3(-1.0f, -1.0f, 1.0f) * scale);
    positions.push_back(Vector3(1.0f, -1.0f, 1.0f) * scale);
    positions.push_back(Vector3(1.0f, 1.0f, 1.0f) * scale);
    positions.push_back(Vector3(-1.0f, 1.0f, 1.0f) * scale);
    colors.push_back(Vector3(0.0f, 1.0f, 1.0f));
    colors.push_back(Vector3(0.0f, 1.0f, 1.0f));
    colors.push_back(Vector3(0.0f, 1.0f, 1.0f));
    colors.push_back(Vector3(0.0f, 1.0f, 1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, 1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, 1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, 1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, 1.0f));
    texcoords.push_back(Vector2(0.0f, 0.0f));
    texcoords.push_back(Vector2(1.0f, 0.0f));
    texcoords.push_back(Vector2(1.0f, 1.0f));
    texcoords.push_back(Vector2(0.0f, 1.0f));

    // 왼쪽
    positions.push_back(Vector3(-1.0f, -1.0f, 1.0f) * scale);
    positions.push_back(Vector3(-1.0f, 1.0f, 1.0f) * scale);
    positions.push_back(Vector3(-1.0f, 1.0f, -1.0f) * scale);
    positions.push_back(Vector3(-1.0f, -1.0f, -1.0f) * scale);
    colors.push_back(Vector3(1.0f, 1.0f, 0.0f));
    colors.push_back(Vector3(1.0f, 1.0f, 0.0f));
    colors.push_back(Vector3(1.0f, 1.0f, 0.0f));
    colors.push_back(Vector3(1.0f, 1.0f, 0.0f));
    normals.push_back(Vector3(-1.0f, 0.0f, 0.0f));
    normals.push_back(Vector3(-1.0f, 0.0f, 0.0f));
    normals.push_back(Vector3(-1.0f, 0.0f, 0.0f));
    normals.push_back(Vector3(-1.0f, 0.0f, 0.0f));
    texcoords.push_back(Vector2(0.0f, 0.0f));
    texcoords.push_back(Vector2(1.0f, 0.0f));
    texcoords.push_back(Vector2(1.0f, 1.0f));
    texcoords.push_back(Vector2(0.0f, 1.0f));

    // 오른쪽
    positions.push_back(Vector3(1.0f, -1.0f, 1.0f) * scale);
    positions.push_back(Vector3(1.0f, -1.0f, -1.0f) * scale);
    positions.push_back(Vector3(1.0f, 1.0f, -1.0f) * scale);
    positions.push_back(Vector3(1.0f, 1.0f, 1.0f) * scale);
    colors.push_back(Vector3(1.0f, 0.0f, 1.0f));
    colors.push_back(Vector3(1.0f, 0.0f, 1.0f));
    colors.push_back(Vector3(1.0f, 0.0f, 1.0f));
    colors.push_back(Vector3(1.0f, 0.0f, 1.0f));
    normals.push_back(Vector3(1.0f, 0.0f, 0.0f));
    normals.push_back(Vector3(1.0f, 0.0f, 0.0f));
    normals.push_back(Vector3(1.0f, 0.0f, 0.0f));
    normals.push_back(Vector3(1.0f, 0.0f, 0.0f));
    texcoords.push_back(Vector2(0.0f, 0.0f));
    texcoords.push_back(Vector2(1.0f, 0.0f));
    texcoords.push_back(Vector2(1.0f, 1.0f));
    texcoords.push_back(Vector2(0.0f, 1.0f));

    MeshData meshData;
    for (size_t i = 0; i < positions.size(); i++) {
        Vertex v;
        v.position  = positions[i];
        v.color = colors[i];
        v.normal    = normals[i];
        v.texcoord  = texcoords[i];
        meshData.vertices.push_back(v);
    }

    meshData.indices = {
        0,  1,  2,  0,  2,  3,  // 윗면
        4,  5,  6,  4,  6,  7,  // 아랫면
        8,  9,  10, 8,  10, 11, // 앞면
        12, 13, 14, 12, 14, 15, // 뒷면
        16, 17, 18, 16, 18, 19, // 왼쪽
        20, 21, 22, 20, 22, 23  // 오른쪽
    };

    return meshData;
}

//std::vector<MeshData> GeometryGenerator::ReadFromFile(std::string filePath) {
//
//    using namespace DirectX;
//
//    ModelLoader modelLoader;
//    modelLoader.Load(filePath);
//    std::vector<MeshData>& meshes = modelLoader.meshes;
//
//    // Normalize vertices
//    Vector3 vmin(1000, 1000, 1000);
//    Vector3 vmax(-1000, -1000, -1000);
//    for (auto& mesh : meshes) {
//        for (auto& v : mesh.vertices) {
//            vmin.x = XMMin(vmin.x, v.position.x);
//            vmin.y = XMMin(vmin.y, v.position.y);
//            vmin.z = XMMin(vmin.z, v.position.z);
//            vmax.x = XMMax(vmax.x, v.position.x);
//            vmax.y = XMMax(vmax.y, v.position.y);
//            vmax.z = XMMax(vmax.z, v.position.z);
//        }
//    }
//
//    float dx = vmax.x - vmin.x, dy = vmax.y - vmin.y, dz = vmax.z - vmin.z;
//    float dl = XMMax(XMMax(dx, dy), dz);
//    float cx = (vmax.x + vmin.x) * 0.5f, cy = (vmax.y + vmin.y) * 0.5f,
//        cz = (vmax.z + vmin.z) * 0.5f;
//
//    for (auto& mesh : meshes) {
//        for (auto& v : mesh.vertices) {
//            v.position.x = (v.position.x - cx) / dl;
//            v.position.y = (v.position.y - cy) / dl;
//            v.position.z = (v.position.z - cz) / dl;
//        }
//    }
//
//    return meshes;
//}