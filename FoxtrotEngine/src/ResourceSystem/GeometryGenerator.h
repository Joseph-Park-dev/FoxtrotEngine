// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Generates the FTMeshData that is used to make Primitive Meshes.
/// </summary>

#pragma once
#include "ResourceSystem/FTMeshData.h"
#include "ResourceSystem/FTTileMap.h"

class Tile;
struct TextAttribute;

class GeometryGenerator
{
public:
	static FTMeshData MakeSquare();
	static FTMeshData MakeSquare(float scale);
	static FTMeshData MakeSquare(FTVector3 color);
	static FTMeshData MakeSquare(float scale, FTVector3 color);

	// MakeSquare() for Making tilemap
	static std::vector<FTMeshData> MakeTileMapGrid(FTTileMap* tileMap);

	static FTMeshData MakeTile(Tile& tile);
	static void		  MakeSpriteAnimation(std::vector<FTMeshData>& animMeshes, Tile* tileMap, size_t columnCount, size_t rowCount);
	static FTMeshData MakeAnimationFrame(Tile tile);
	// static void MakeSpriteTextGrid(std::vector<FTMeshData>& textMeshes, Tile* tileMap, size_t length, TextAttribute* attribute);
	// static FTMeshData MakeSpriteText(Tile tile, TextAttribute* attrib, size_t col, size_t row);

	// Make 3D Models
	static FTMeshData MakeBox();
	static FTMeshData MakeSquareGrid(const float width, const float height, const int numSlices, const int numStacks);
	static FTMeshData MakeCylinder(const float bottomRadius, const float topRadius, float height, int sliceCount);
	static FTMeshData MakeSphere(const float radius, const int numSlices, const int numStacks);

	static std::vector<FTMeshData> ReadFromFile(std::string resPath);
	static std::vector<FTMeshData> ReadFromFile(std::string basePath, std::string filename);
};