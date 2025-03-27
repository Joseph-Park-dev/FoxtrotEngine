// ----------------------------------------------------------------
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
#include "ResourceSystem/FTMeshData.h"
#include "ResourceSystem/FTTexture.h"
#include "ResourceSystem/FTTileMap.h"
#include "ResourceSystem/Tile.h"
#include "ResourceSystem/TextAttribute.h"
#include "ResourceSystem/ModelLoader.h"

// #include "ModelLoader.h"
// using namespace DirectX;
// using namespace DirectX::SimpleMath;
// using D3DVec2 = DirectX::SimpleMath::Vector2;
// using D3DVec3 = DirectX::SimpleMath::Vector3;

using Vector3 = DirectX::SimpleMath::Vector3;
using Vector2 = DirectX::SimpleMath::Vector2;
using Matrix  = DirectX::SimpleMath::Matrix;

FTMeshData GeometryGenerator::MakeSquare()
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

	FTMeshData meshData;
	for (size_t i = 0; i < positions.size(); i++)
	{
		Vertex v;
		v.position = positions[i];
		// v.color = colors[i];
		v.normal   = normals[i];
		v.texcoord = texcoords[i];
		meshData.Vertices.push_back(v);
	}
	meshData.Indices = {
		0, 1, 2, 0, 2, 3, // 앞면
	};

	return meshData;
}

FTMeshData GeometryGenerator::MakeSquare(float scale)
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

	FTMeshData meshData;
	for (size_t i = 0; i < positions.size(); i++)
	{
		Vertex v;
		v.position = positions[i];
		// v.color = colors[i];
		v.normal   = normals[i];
		v.texcoord = texcoords[i];
		meshData.Vertices.push_back(v);
	}
	meshData.Indices = {
		0, 1, 2, 0, 2, 3, // 앞면
	};

	return meshData;
}

FTMeshData GeometryGenerator::MakeSquare(FTVector3 color)
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

	FTMeshData meshData;
	for (size_t i = 0; i < positions.size(); i++)
	{
		Vertex v;
		v.position = positions[i];
		// v.color = colors[i];
		v.normal   = normals[i];
		v.texcoord = texcoords[i];
		meshData.Vertices.push_back(v);
	}
	meshData.Indices = {
		0, 1, 2, 0, 2, 3, // 앞면
	};

	return meshData;
}

FTMeshData GeometryGenerator::MakeSquare(float scale, FTVector3 color)
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

	FTMeshData meshData;
	for (size_t i = 0; i < positions.size(); i++)
	{
		Vertex v;
		v.position = positions[i];
		// v.color = colors[i];
		v.normal   = normals[i];
		v.texcoord = texcoords[i];
		meshData.Vertices.push_back(v);
	}
	meshData.Indices = {
		0, 1, 2, 0, 2, 3, // 앞면
	};

	return meshData;
}

std::vector<FTMeshData> GeometryGenerator::MakeTileMapGrid(FTTileMap* tileMap)
{
	std::vector<FTMeshData> tileMapMeshes;
	LogInt("X : ", tileMap->GetMaxCountOnScreenX());
	LogInt("Y : ", tileMap->GetMaxCountOnScreenY());
	for (size_t r = 0; r < tileMap->GetMaxCountOnScreenY(); ++r)
	{
		for (size_t c = 0; c < tileMap->GetMaxCountOnScreenX(); ++c)
		{
			Tile&	   tile		= tileMap->GetTile(r, c);
			FTMeshData tileMesh = MakeTile(tile);
			tileMapMeshes.push_back(tileMesh);
		}
	}
	return tileMapMeshes;
}

FTMeshData GeometryGenerator::MakeTile(Tile& tile)
{
	std::vector<Vector3> positions;
	std::vector<Vector3> colors;
	std::vector<Vector3> normals;
	std::vector<Vector2> texcoords; // 텍스춰 좌표

	FTRectArea& rectOnScreen = tile.GetRectOnScreen();

	const FTVector2& tileMin	= rectOnScreen.GetMin();
	const FTVector2& tileMax	= rectOnScreen.GetMax();
	const float		 tileWidth	= rectOnScreen.GetSize().x;
	const float		 tileHeight = rectOnScreen.GetSize().y;

	// 앞면
	positions.push_back(Vector3(tileMin.x, -tileMin.y, 0.0f));
	positions.push_back(Vector3(tileMin.x + tileWidth, -tileMin.y, 0.0f));
	positions.push_back(Vector3(tileMin.x + tileWidth, -tileMin.y - tileHeight, 0.0f));
	positions.push_back(Vector3(tileMin.x, -tileMin.y - tileHeight, 0.0f));
	colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
	colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
	colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
	colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
	normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
	normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
	normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
	normals.push_back(Vector3(0.0f, 0.0f, -1.0f));

	// Texture Coordinates
	FTRectArea&		 rectOnMap	 = tile.GetRectOnMap();
	const FTVector2& mapMin		 = rectOnMap.GetMin();
	const float		 widthInMap	 = rectOnMap.GetSize().x;
	const float		 heightInMap = rectOnMap.GetSize().y;

	texcoords.push_back(Vector2(mapMin.x, mapMin.y));
	texcoords.push_back(Vector2(mapMin.x + widthInMap, mapMin.y));
	texcoords.push_back(Vector2(mapMin.x + widthInMap, mapMin.y + heightInMap));
	texcoords.push_back(Vector2(mapMin.x, mapMin.y + heightInMap));

	FTMeshData meshData;
	for (size_t i = 0; i < positions.size(); i++)
	{
		Vertex v;
		v.position = positions[i];
		// v.color = colors[i];
		v.normal   = normals[i];
		v.texcoord = texcoords[i];
		meshData.Vertices.push_back(v);
	}
	meshData.Indices = {
		0, 1, 2, 0, 2, 3, // 앞면
	};

	return meshData;
}

void GeometryGenerator::MakeSpriteAnimation(std::vector<FTMeshData>& animMeshes, Tile* tileMap, size_t columnCount, size_t rowCount)
{
	for (size_t r = 0; r < rowCount; ++r)
	{
		for (size_t c = 0; c < columnCount; ++c)
		{
			size_t	   idx		 = columnCount * r + c;
			FTMeshData animFrame = MakeAnimationFrame(tileMap[columnCount * r + c]);
			animMeshes.push_back(animFrame);
		}
	}
}

FTMeshData GeometryGenerator::MakeAnimationFrame(Tile tile)
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
	FTRectArea&		 rectOnMap	 = tile.GetRectOnMap();
	const FTVector2& mapMin		 = rectOnMap.GetMin();
	const float		 widthInMap	 = rectOnMap.GetSize().x;
	const float		 heightInMap = rectOnMap.GetSize().y;

	texcoords.push_back(Vector2(mapMin.x, mapMin.y));
	texcoords.push_back(Vector2(mapMin.x + widthInMap, mapMin.y));
	texcoords.push_back(Vector2(mapMin.x + widthInMap, mapMin.y + heightInMap));
	texcoords.push_back(Vector2(mapMin.x, mapMin.y + heightInMap));

	FTMeshData meshData;
	for (size_t i = 0; i < positions.size(); i++)
	{
		Vertex v;
		v.position = positions[i];
		// v.color = colors[i];
		v.normal   = normals[i];
		v.texcoord = texcoords[i];
		meshData.Vertices.push_back(v);
	}
	meshData.Indices = {
		0, 1, 2, 0, 2, 3, // 앞면
	};

	return meshData;
}

// void GeometryGenerator::MakeSpriteTextGrid(std::vector<FTMeshData>& textMeshes, Tile* tileMap, size_t length, TextAttribute* attribute)
//{
//     if (0 < attribute->MaxChars)
//     {
//         size_t maxRow = length / attribute->MaxChars < 1 ? 1 : length / attribute->MaxChars;
//         for (size_t i = 0; i <= maxRow; ++i)
//         {
//             for (size_t j = 0; j < attribute->MaxChars; ++j)
//             {
//                 size_t idx = attribute->MaxChars * i + j;
//                 FTMeshData spriteText = MakeSpriteText(tileMap[idx], attribute, j, i);
//                 textMeshes.push_back(spriteText);
//             }
//         }
//     }
// }
//
// FTMeshData GeometryGenerator::MakeSpriteText(Tile tile, TextAttribute* attrib, size_t col, size_t row)
//{
//     std::vector<Vector3> positions;
//     std::vector<Vector3> colors;
//     std::vector<Vector3> normals;
//     std::vector<Vector2> texcoords; // 텍스춰 좌표
//
//     FTRectArea& rectOnScreen = tile.GetRectOnScreen();
//
//     const float tileWidth = rectOnScreen.GetSize().x * attrib->Scale;
//     const float tileHeight = rectOnScreen.GetSize().y * attrib->Scale;
//     FTVector2 tileMin = FTVector2(tileWidth * col, tileHeight * row) * FTVector2(attrib->CharSpacing, 1.f);
//
//     // 앞면
//     positions.push_back(Vector3(tileMin.x,             -tileMin.y,              0.0f));
//     positions.push_back(Vector3(tileMin.x + tileWidth, -tileMin.y,              0.0f));
//     positions.push_back(Vector3(tileMin.x + tileWidth, -tileMin.y - tileHeight, 0.0f));
//     positions.push_back(Vector3(tileMin.x,             -tileMin.y - tileHeight, 0.0f));
//     colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
//     colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
//     colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
//     colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
//     normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
//     normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
//     normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
//     normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
//
//     // Texture Coordinates (Direct3D 9)
//     // https://learn.microsoft.com/en-us/windows/win32/direct3d9/texture-coordinates
//     FTRectArea& rectOnMap = tile.GetRectOnMap();
//     const FTVector2& mapMin = rectOnMap.GetMin();
//     const float widthInMap = rectOnMap.GetSize().x;
//     const float heightInMap = rectOnMap.GetSize().y;
//
//     texcoords.push_back(Vector2(mapMin.x, mapMin.y));
//     texcoords.push_back(Vector2(mapMin.x + widthInMap, mapMin.y));
//     texcoords.push_back(Vector2(mapMin.x + widthInMap, mapMin.y + heightInMap));
//     texcoords.push_back(Vector2(mapMin.x, mapMin.y + heightInMap));
//
//     FTMeshData meshData;
//     for (size_t i = 0; i < positions.size(); i++) {
//         Vertex v;
//         v.position = positions[i];
//         v.color = colors[i];
//         v.normal = normals[i];
//         v.texcoord = texcoords[i];
//         meshData.Vertices.push_back(v);
//     }
//     meshData.Indices = {
//         0, 1, 2, 0, 2, 3, // 앞면
//     };
//
//     return meshData;
// }

FTMeshData GeometryGenerator::MakeBox()
{
	return MakeBox(0.1f);
}

FTMeshData GeometryGenerator::MakeBox(float scale)
{
	std::vector<Vector3> positions;
	std::vector<Vector3> colors;
	std::vector<Vector3> normals;
	std::vector<Vector2> texcoords; // 텍스춰 좌표

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

	FTMeshData meshData;
	for (size_t i = 0; i < positions.size(); i++)
	{
		Vertex v;
		v.position = positions[i];
		// v.color     = colors[i];
		v.normal = normals[i];
		v.texcoord = texcoords[i];
		meshData.Vertices.push_back(v);
	}

	meshData.Indices = {
		0, 1, 2, 0, 2, 3, // Up
		4,
		5,
		6,
		4,
		6,
		7, // Down
		8,
		9,
		10,
		8,
		10,
		11, // Front
		12,
		13,
		14,
		12,
		14,
		15, // Back
		16,
		17,
		18,
		16,
		18,
		19, // Left
		20,
		21,
		22,
		20,
		22,
		23 // Right
	};

	return meshData;
}

FTMeshData GeometryGenerator::MakeSquareGrid(
	const float width,
	const float height,
	const int	numSlices,
	const int	numStacks)
{
	FTMeshData meshData;

	const float dx = width / numSlices;
	const float dy = height / numStacks;

	std::vector<Vertex>&   vertices = meshData.Vertices;
	std::vector<uint32_t>& indices	= meshData.Indices;

	Vector3 leftBottom = Vector3(-0.5f * width, -0.5f * height, 0.0f);

	for (size_t j = 0; j <= numStacks; ++j)
	{
		Vector3 startStackPoint =
			Vector3::Transform(leftBottom, Matrix::CreateTranslation(0.0f, dy * j, 0.0f));

		for (size_t i = 0; i <= numSlices; ++i)
		{
			Vertex v;
			v.position = Vector3::Transform(startStackPoint, Matrix::CreateTranslation(dx * i, 0.0f, 0.0f));
			v.normal   = -Vector3::UnitZ;
			v.texcoord = Vector2(float(i) / numSlices, 1.0f - float(j) / numStacks);
			vertices.push_back(v);
		}
	}

	for (int j = 0; j < numStacks; ++j)
	{
		const int offset = (numSlices + 1) * j;

		for (int i = 0; i < numSlices; ++i)
		{
			indices.push_back(offset + i);
			indices.push_back(offset + i + numSlices + 1);
			indices.push_back(offset + i + 1 + numSlices + 1);

			indices.push_back(offset + i);
			indices.push_back(offset + i + 1 + numSlices + 1);
			indices.push_back(offset + i + 1);
		}
	}

	return meshData;
}

FTMeshData GeometryGenerator::MakeCylinder(const float bottomRadius, const float topRadius, float height, int sliceCount)
{
	using namespace DirectX;
	using DirectX::SimpleMath::Matrix;
	using DirectX::SimpleMath::Vector3;

	// Texture 좌표계때문에 (sliceCount + 1) x 2 개의 버텍스 사용

	const float dTheta = -XM_2PI / float(sliceCount);

	FTMeshData meshData;

	std::vector<Vertex>& vertices = meshData.Vertices;

	// 옆면의 바닥 버텍스들 (인덱스 0 이상 sliceCount 미만)
	for (int i = 0; i <= sliceCount; i++)
	{
		Vertex v;

		// TODO: 작성 (텍스춰 좌표계, 버텍스 노멀 필요)
		v.position = Vector3::Transform(Vector3(bottomRadius, -0.5f * height, 0.0f), Matrix::CreateRotationY(dTheta * float(i)));
		v.normal   = v.position - Vector3(0.0f, -0.5f * height, 0.0f);
		v.normal.Normalize();
		v.texcoord = Vector2(float(i) / sliceCount, 0.0f);

		vertices.push_back(v);
	}

	// 옆면의 맨 위 버텍스들 (인덱스 sliceCount 이상 2 * sliceCount 미만)
	for (int i = 0; i <= sliceCount; i++)
	{
		Vertex v;

		// TODO: 작성 (텍스춰 좌표계, 버텍스 노멀 필요)
		v.position = Vector3::Transform(Vector3(bottomRadius, 0.5f * height, 0.0f), Matrix::CreateRotationY(dTheta * float(i)));
		v.normal   = v.position - Vector3(0.0f, 0.5f * height, 0.0f);
		v.normal.Normalize();
		v.texcoord = Vector2(float(i) / sliceCount, 1.0f);

		vertices.push_back(v);
	}

	std::vector<uint32_t>& indices = meshData.Indices;

	for (int i = 0; i < sliceCount; i++)
	{
		// TODO: 삼각형 두 개 씩
		indices.push_back(i);
		indices.push_back(i + sliceCount + 1);
		indices.push_back(i + sliceCount + 1 + 1);

		indices.push_back(i);
		indices.push_back(i + 1 + sliceCount + 1);
		indices.push_back(i + 1);
	}

	return meshData;
}

FTMeshData GeometryGenerator::MakeSphere(const float radius, const int numSlices, const int numStacks)
{
	using namespace DirectX;

	const float dTheta = -XM_2PI / float(numSlices);
	const float dPhi   = -XM_PI / float(numStacks);

	FTMeshData meshData;

	std::vector<Vertex>& vertices = meshData.Vertices;

	for (int j = 0; j <= numStacks; j++)
	{

		Vector3 stackStartPoint =
			Vector3::Transform(
				Vector3(0.0f, -radius, 0.0f), Matrix::CreateRotationZ(dPhi * float(j)));

		for (int i = 0; i <= numSlices; i++)
		{
			Vertex v;

			v.position = Vector3::Transform(
				stackStartPoint, Matrix::CreateRotationY(dTheta * float(i)));
			v.normal = v.position;
			v.normal.Normalize();
			v.texcoord = Vector2(float(i) / numSlices, 1 - float(j) / numStacks);

			vertices.push_back(v);
		}
	}

	std::vector<uint32_t>& indices = meshData.Indices;

	for (int j = 0; j < numStacks; j++)
	{

		const int offset = (numSlices + 1) * j;

		for (int i = 0; i < numSlices; i++)
		{

			indices.push_back(offset + i);
			indices.push_back(offset + i + numSlices + 1);
			indices.push_back(offset + i + 1 + numSlices + 1);

			indices.push_back(offset + i);
			indices.push_back(offset + i + 1 + numSlices + 1);
			indices.push_back(offset + i + 1);
		}
	}

	return meshData;
}

std::vector<FTMeshData> GeometryGenerator::ReadFromFile(std::string resPath)
{
	using namespace DirectX;

	ModelLoader modelLoader;
	modelLoader.Load(resPath);
	std::vector<FTMeshData>& meshes = modelLoader.meshes;

	// Normalize Vertices
	Vector3 vmin(1000, 1000, 1000);
	Vector3 vmax(-1000, -1000, -1000);
	for (auto& mesh : meshes)
	{
		for (auto& v : mesh.Vertices)
		{
			vmin.x = XMMin(vmin.x, v.position.x);
			vmin.y = XMMin(vmin.y, v.position.y);
			vmin.z = XMMin(vmin.z, v.position.z);
			vmax.x = XMMax(vmax.x, v.position.x);
			vmax.y = XMMax(vmax.y, v.position.y);
			vmax.z = XMMax(vmax.z, v.position.z);
		}
	}

	float dx = vmax.x - vmin.x, dy = vmax.y - vmin.y, dz = vmax.z - vmin.z;
	float dl = XMMax(XMMax(dx, dy), dz);
	float cx = (vmax.x + vmin.x) * 0.5f, cy = (vmax.y + vmin.y) * 0.5f,
		  cz = (vmax.z + vmin.z) * 0.5f;

	for (auto& mesh : meshes)
	{
		for (auto& v : mesh.Vertices)
		{
			v.position.x = (v.position.x - cx) / dl;
			v.position.y = (v.position.y - cy) / dl;
			v.position.z = (v.position.z - cz) / dl;
		}
	}

	return meshes;
}

std::vector<FTMeshData> GeometryGenerator::ReadFromFile(std::string basePath, std::string filename)
{
	using namespace DirectX;

	ModelLoader modelLoader;
	modelLoader.Load(basePath, filename);
	std::vector<FTMeshData>& meshes = modelLoader.meshes;

	// Normalize Vertices
	Vector3 vmin(1000, 1000, 1000);
	Vector3 vmax(-1000, -1000, -1000);
	for (auto& mesh : meshes)
	{
		for (auto& v : mesh.Vertices)
		{
			vmin.x = XMMin(vmin.x, v.position.x);
			vmin.y = XMMin(vmin.y, v.position.y);
			vmin.z = XMMin(vmin.z, v.position.z);
			vmax.x = XMMax(vmax.x, v.position.x);
			vmax.y = XMMax(vmax.y, v.position.y);
			vmax.z = XMMax(vmax.z, v.position.z);
		}
	}

	float dx = vmax.x - vmin.x, dy = vmax.y - vmin.y, dz = vmax.z - vmin.z;
	float dl = XMMax(XMMax(dx, dy), dz);
	float cx = (vmax.x + vmin.x) * 0.5f, cy = (vmax.y + vmin.y) * 0.5f,
		  cz = (vmax.z + vmin.z) * 0.5f;

	for (auto& mesh : meshes)
	{
		for (auto& v : mesh.Vertices)
		{
			v.position.x = (v.position.x - cx) / dl;
			v.position.y = (v.position.y - cy) / dl;
			v.position.z = (v.position.z - cz) / dl;
		}
	}

	return meshes;
}