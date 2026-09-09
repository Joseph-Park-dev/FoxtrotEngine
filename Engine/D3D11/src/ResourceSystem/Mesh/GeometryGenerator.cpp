// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "ResourceSystem/Mesh/GeometryGenerator.h"

#include "TemplateFunctions.h"
#include "FTMath.h"
#include "ResourceSystem/Mesh/Vertex.h"
#include "ResourceSystem/Mesh/FTMeshData.h"
#include "ResourceSystem/FTTexture.h"
#include "ResourceSystem/Sprite/FTTileMap.h"
#include "ResourceSystem/Sprite/Tile.h"
#include "ResourceSystem/Font/TextAttribute.h"
#include "ResourceSystem/Font/TextVertex.h"

#include "FTDS/Static/FTString.h"

namespace D3D11
{
	using namespace Common;
	using namespace Core;
	using namespace Math;
	
	/// @brief Builds mesh data representing a square.
	/// @return Created square instance or resource.
	FTMeshData GeometryGenerator::MakeSquare()
	{
		Common::FTDS::DynamicArray<FTVector3> positions;
		Common::FTDS::DynamicArray<FTVector4> colors;
		Common::FTDS::DynamicArray<FTVector2> texcoords;

		positions.Reserve(4);
		colors.Reserve(4);
		texcoords.Reserve(4);

		positions.PushBack(FTVector3(-0.5f, 0.5f, 0.0f));
		positions.PushBack(FTVector3(0.5f, 0.5f, 0.0f));
		positions.PushBack(FTVector3(0.5f, -0.5f, 0.0f));
		positions.PushBack(FTVector3(-0.5f, -0.5f, 0.0f));

		colors.PushBack(FTVector4(0.0f, 0.0f, 0.0f, 1.0f));
		colors.PushBack(FTVector4(0.0f, 0.0f, 0.0f, 1.0f));
		colors.PushBack(FTVector4(0.0f, 0.0f, 0.0f, 1.0f));
		colors.PushBack(FTVector4(0.0f, 0.0f, 0.0f, 1.0f));

		texcoords.PushBack(FTVector2(0.0f, 0.0f));
		texcoords.PushBack(FTVector2(1.0f, 0.0f));
		texcoords.PushBack(FTVector2(1.0f, 1.0f));
		texcoords.PushBack(FTVector2(0.0f, 1.0f));

		FTMeshData meshData;
		for (size_t i = 0; i < positions.GetSize(); i++)
		{
			Vertex v;
			v.position = positions[i];
			v.color	   = colors[i];
			v.texcoord = texcoords[i];
			meshData.Vertices.PushBack(v);
		}

		meshData.Indices.Reserve(6);

		meshData.Indices.PushBack(0);
		meshData.Indices.PushBack(1);
		meshData.Indices.PushBack(2);
		meshData.Indices.PushBack(0);
		meshData.Indices.PushBack(2);
		meshData.Indices.PushBack(3);

		return meshData;
	}

	/// @brief Builds mesh data representing a square.
	/// @param scale Scale factor applied to the content.
	/// @return Created square instance or resource.
	FTMeshData GeometryGenerator::MakeSquare(float scale)
	{
		Common::FTDS::DynamicArray<FTVector3> positions;
		Common::FTDS::DynamicArray<FTVector4> colors;
		Common::FTDS::DynamicArray<FTVector2> texcoords;

		positions.Reserve(4);
		colors.Reserve(4);
		texcoords.Reserve(4);

		positions.PushBack(FTVector3(-0.5f, 0.5f, 0.0f) * scale);
		positions.PushBack(FTVector3(0.5f, 0.5f, 0.0f) * scale);
		positions.PushBack(FTVector3(0.5f, -0.5f, 0.0f) * scale);
		positions.PushBack(FTVector3(-0.5f, -0.5f, 0.0f) * scale);

		colors.PushBack(FTVector4(0.0f, 0.0f, 0.0f, 1.0f));
		colors.PushBack(FTVector4(0.0f, 0.0f, 0.0f, 1.0f));
		colors.PushBack(FTVector4(0.0f, 0.0f, 0.0f, 1.0f));
		colors.PushBack(FTVector4(0.0f, 0.0f, 0.0f, 1.0f));

		texcoords.PushBack(FTVector2(0.0f, 0.0f));
		texcoords.PushBack(FTVector2(1.0f, 0.0f));
		texcoords.PushBack(FTVector2(1.0f, 1.0f));
		texcoords.PushBack(FTVector2(0.0f, 1.0f));

		FTMeshData meshData;
		for (size_t i = 0; i < positions.GetSize(); i++)
		{
			Vertex v;
			v.position = positions[i];
			v.color	   = colors[i];
			v.texcoord = texcoords[i];
			meshData.Vertices.PushBack(v);
		}

		meshData.Indices.Reserve(6);

		meshData.Indices.PushBack(0);
		meshData.Indices.PushBack(1);
		meshData.Indices.PushBack(2);
		meshData.Indices.PushBack(0);
		meshData.Indices.PushBack(2);
		meshData.Indices.PushBack(3);

		return meshData;
	}

	/// @brief Builds mesh data representing a point.
	/// @param color Color components used when rendering.
	/// @return Created point instance or resource.
	FTMeshData GeometryGenerator::MakePoint(FTVector4 color)
	{
		FTMeshData meshData;

		Vertex vertex;
		vertex.position = FTVector3::Zero;
		vertex.color	= FTVector4(color.x, color.y, color.z, 1.0f);
		vertex.texcoord = FTVector2::Zero;

		meshData.Vertices.PushBack(vertex);

		return meshData;
	}

	/// @brief Creates point geometry for editor or debug visualization.
	/// @param color Color components used when rendering.
	/// @return Created debug point instance or resource.
	FTDebugMeshData GeometryGenerator::MakeDebugPoint(FTVector4 color)
	{
		FTDebugMeshData meshData;

		DebugVertex vertex;
		vertex.position = FTVector3::Zero;
		vertex.color	= color;

		meshData.Vertices.PushBack(vertex);

		return meshData;
	}

	/// @brief Builds mesh data representing a square.
	/// @param scale Scale factor applied to the content.
	/// @param color Color components used when rendering.
	/// @return Created square instance or resource.
	FTMeshData* GeometryGenerator::MakeSquare(float scale, FTVector4 color)
	{
		Common::FTDS::DynamicArray<FTVector3> positions;
		Common::FTDS::DynamicArray<FTVector4> colors;
		Common::FTDS::DynamicArray<FTVector2> texcoords;

		positions.Reserve(4);
		colors.Reserve(4);
		texcoords.Reserve(4);

		positions.PushBack(FTVector3(-0.5f, 0.5f, 0.0f) * scale);
		positions.PushBack(FTVector3(0.5f, 0.5f, 0.0f) * scale);
		positions.PushBack(FTVector3(0.5f, -0.5f, 0.0f) * scale);
		positions.PushBack(FTVector3(-0.5f, -0.5f, 0.0f) * scale);

		colors.PushBack(color);
		colors.PushBack(color);
		colors.PushBack(color);
		colors.PushBack(color);

		texcoords.PushBack(FTVector2(0.0f, 0.0f));
		texcoords.PushBack(FTVector2(1.0f, 0.0f));
		texcoords.PushBack(FTVector2(1.0f, 1.0f));
		texcoords.PushBack(FTVector2(0.0f, 1.0f));

		FTMeshData* meshData = DBG_NEW FTMeshData;
		for (size_t i = 0; i < positions.GetSize(); i++)
		{
			Vertex v;
			v.position = positions[i];
			v.color	   = colors[i];
			// v.normal   = normals[i];
			v.texcoord = texcoords[i];
			meshData->Vertices.PushBack(v);
		}

		meshData->Indices.Reserve(6);

		meshData->Indices.PushBack(0);
		meshData->Indices.PushBack(1);
		meshData->Indices.PushBack(2);
		meshData->Indices.PushBack(0);
		meshData->Indices.PushBack(2);
		meshData->Indices.PushBack(3);

		return meshData;
	}

	/// @brief Builds the grid geometry used to render a tile map.
	/// @param tileMap Tile-map resource used for rendering.
	/// @return Created tile map grid instance or resource.
	Common::FTDS::DynamicArray<FTMeshData> GeometryGenerator::MakeTileMapGrid(FTTileMap* tileMap)
	{
		Common::FTDS::DynamicArray<FTMeshData> tileMapMeshes;
		LogInt("X : ", tileMap->GetMaxCountOnScreenX());
		LogInt("Y : ", tileMap->GetMaxCountOnScreenY());
		for (size_t r = 0; r < tileMap->GetMaxCountOnScreenY(); ++r)
		{
			for (size_t c = 0; c < tileMap->GetMaxCountOnScreenX(); ++c)
			{
				Tile&	   tile		= tileMap->GetTile(r, c);
				FTMeshData tileMesh = MakeTile(tile);
				tileMapMeshes.PushBack(tileMesh);
			}
		}
		return tileMapMeshes;
	}

	/// @brief Builds geometry and texture coordinates for one tile.
	/// @param tile Tile instance to process.
	/// @return Created tile instance or resource.
	FTMeshData GeometryGenerator::MakeTile(Tile& tile)
	{
		Common::FTDS::DynamicArray<FTVector3> positions;
		Common::FTDS::DynamicArray<FTVector4> colors;
		Common::FTDS::DynamicArray<FTVector2> texcoords; // 텍스춰 좌표

		FTRectArea* rectOnScreen = tile.GetRectOnScreen();

		const FTVector2& tileMin	= rectOnScreen->GetMin();
		const FTVector2& tileMax	= rectOnScreen->GetMax();
		const float		 tileWidth	= rectOnScreen->GetSize().x;
		const float		 tileHeight = rectOnScreen->GetSize().y;

		// 앞면
		positions.PushBack(FTVector3(tileMin.x, -tileMin.y, 0.0f));
		positions.PushBack(FTVector3(tileMin.x + tileWidth, -tileMin.y, 0.0f));
		positions.PushBack(FTVector3(tileMin.x + tileWidth, -tileMin.y - tileHeight, 0.0f));
		positions.PushBack(FTVector3(tileMin.x, -tileMin.y - tileHeight, 0.0f));
		colors.PushBack(FTVector4(0.0f, 0.0f, 0.0f, 1.0f));
		colors.PushBack(FTVector4(0.0f, 0.0f, 0.0f, 1.0f));
		colors.PushBack(FTVector4(0.0f, 0.0f, 0.0f, 1.0f));
		colors.PushBack(FTVector4(0.0f, 0.0f, 0.0f, 1.0f));

		// Texture Coordinates
		FTRectArea*		 rectOnMap	 = tile.GetRectOnMap();
		const FTVector2& mapMin		 = rectOnMap->GetMin();
		const float		 widthInMap	 = rectOnMap->GetSize().x;
		const float		 heightInMap = rectOnMap->GetSize().y;

		texcoords.PushBack(FTVector2(mapMin.x, mapMin.y));
		texcoords.PushBack(FTVector2(mapMin.x + widthInMap, mapMin.y));
		texcoords.PushBack(FTVector2(mapMin.x + widthInMap, mapMin.y + heightInMap));
		texcoords.PushBack(FTVector2(mapMin.x, mapMin.y + heightInMap));

		FTMeshData meshData;
		for (size_t i = 0; i < positions.GetSize(); i++)
		{
			Vertex v;
			v.position = positions[i];
			v.color	   = colors[i];
			v.texcoord = texcoords[i];
			meshData.Vertices.PushBack(v);
		}

		meshData.Indices.Reserve(6);

		meshData.Indices.PushBack(0);
		meshData.Indices.PushBack(1);
		meshData.Indices.PushBack(2);
		meshData.Indices.PushBack(0);
		meshData.Indices.PushBack(2);
		meshData.Indices.PushBack(3);

		return meshData;
	}

	/// @brief Creates a sprite animation from the supplied frame descriptions.
	/// @param animMeshes Meshes associated with the animation frames.
	/// @param tileMap Tile-map resource used for rendering.
	/// @param startIdx Beginning index of the selected range.
	/// @param endIdx Ending index of the selected range.
	void GeometryGenerator::MakeSpriteAnimation(Common::FTDS::DynamicArray<FTMeshData*>& animMeshes, Tile* tileMap, size_t startIdx, size_t endIdx)
	{
		size_t count = endIdx - startIdx + 1;
		assert(0 < count);
		animMeshes.Reserve(count);
		for (size_t i = 0; i < count; ++i)
		{
			FTMeshData* animFrame = MakeAnimationFrame(tileMap[startIdx + i]);
			animMeshes.PushBack(animFrame);
		}
	}

	/// @brief Creates a sprite animation from the supplied frame descriptions.
	/// @param animMeshes Meshes associated with the animation frames.
	/// @param tileMap Tile-map resource used for rendering.
	/// @param count Number of entries to process.
	void GeometryGenerator::MakeSpriteAnimation(Common::FTDS::DynamicArray<FTMeshData*>& animMeshes, Tile* tileMap, size_t count)
	{
		animMeshes.Reserve(count);
		for (size_t i = 0; i < count; ++i)
		{
			FTMeshData* animFrame = MakeAnimationFrame(tileMap[i]);
			animMeshes.PushBack(animFrame);
		}
	}

	/// @brief Creates a frame description for an animation clip.
	/// @param tile Tile instance to process.
	/// @return Created animation frame instance or resource.
	FTMeshData* GeometryGenerator::MakeAnimationFrame(Tile& tile)
	{
		Common::FTDS::DynamicArray<FTVector3> positions;
		Common::FTDS::DynamicArray<FTVector4> colors;
		Common::FTDS::DynamicArray<FTVector2> texcoords;

		FTVector2 GetSize = tile.GetRectOnScreen()->GetSize();

		float halfX = GetSize.x / 2.f;
		float halfY = GetSize.y / 2.f;

		positions.PushBack(FTVector3(-halfX, halfY, 0.0f));
		positions.PushBack(FTVector3(halfX, halfY, 0.0f));
		positions.PushBack(FTVector3(halfX, -halfY, 0.0f));
		positions.PushBack(FTVector3(-halfX, -halfY, 0.0f));
		colors.PushBack(FTVector4(0.0f, 0.0f, 0.0f, 1.0f));
		colors.PushBack(FTVector4(0.0f, 0.0f, 0.0f, 1.0f));
		colors.PushBack(FTVector4(0.0f, 0.0f, 0.0f, 1.0f));
		colors.PushBack(FTVector4(0.0f, 0.0f, 0.0f, 1.0f));

		// Texture coordinates used in sampling the TileMap.
		FTRectArea*		 rectOnMap	 = tile.GetRectOnMap();
		const FTVector2& mapMin		 = rectOnMap->GetMin();
		const float		 widthInMap	 = rectOnMap->GetSize().x;
		const float		 heightInMap = rectOnMap->GetSize().y;

		texcoords.PushBack(FTVector2(mapMin.x, mapMin.y));
		texcoords.PushBack(FTVector2(mapMin.x + widthInMap, mapMin.y));
		texcoords.PushBack(FTVector2(mapMin.x + widthInMap, mapMin.y + heightInMap));
		texcoords.PushBack(FTVector2(mapMin.x, mapMin.y + heightInMap));

		FTMeshData* meshData = DBG_NEW FTMeshData;
		for (size_t i = 0; i < positions.GetSize(); i++)
		{
			Vertex v;
			v.position = positions[i];
			v.color	   = colors[i];
			v.texcoord = texcoords[i];
			meshData->Vertices.PushBack(v);
		}

		// if (tile.GetRotated())
		//{
		//	meshData->Vertices.At(0).texcoord = texcoords.at(1);
		//	meshData->Vertices.At(1).texcoord = texcoords.at(2);
		//	meshData->Vertices.At(2).texcoord = texcoords.at(3);
		//	meshData->Vertices.At(3).texcoord = texcoords.at(0);
		// }

		meshData->Indices.Reserve(6);

		meshData->Indices.PushBack(0);
		meshData->Indices.PushBack(1);
		meshData->Indices.PushBack(2);
		meshData->Indices.PushBack(0);
		meshData->Indices.PushBack(2);
		meshData->Indices.PushBack(3);

		return meshData;
	}
} // namespace D3D11

// void GeometryGenerator::MakeSpriteTextGrid(Common::FTDS::DynamicArray<FTMeshData>& textMeshes, Tile* tileMap, size_t length, TextAttribute* attribute)
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
//                 textMeshes.PushBack(spriteText);
//             }
//         }
//     }
// }
//
// FTMeshData GeometryGenerator::MakeSpriteText(Tile tile, TextAttribute* attrib, size_t col, size_t row)
//{
//     Common::FTDS::DynamicArray<FTVector3> positions;
//     Common::FTDS::DynamicArray<FTVector3> colors;
//     Common::FTDS::DynamicArray<FTVector3> normals;
//     Common::FTDS::DynamicArray<FTVector2> texcoords; // 텍스춰 좌표
//
//     FTRectArea& rectOnScreen = tile.GetRectOnScreen();
//
//     const float tileWidth = rectOnScreen.GetSize().x * attrib->Scale;
//     const float tileHeight = rectOnScreen.GetSize().y * attrib->Scale;
//     FTVector2 tileMin = FTVector2(tileWidth * col, tileHeight * row) * FTVector2(attrib->CharSpacing, 1.f);
//
//     // 앞면
//     positions.PushBack(FTVector3(tileMin.x,             -tileMin.y,              0.0f));
//     positions.PushBack(FTVector3(tileMin.x + tileWidth, -tileMin.y,              0.0f));
//     positions.PushBack(FTVector3(tileMin.x + tileWidth, -tileMin.y - tileHeight, 0.0f));
//     positions.PushBack(FTVector3(tileMin.x,             -tileMin.y - tileHeight, 0.0f));
//     colors.PushBack(FTVector3(0.0f, 0.0f, 1.0f));
//     colors.PushBack(FTVector3(0.0f, 0.0f, 1.0f));
//     colors.PushBack(FTVector3(0.0f, 0.0f, 1.0f));
//     colors.PushBack(FTVector3(0.0f, 0.0f, 1.0f));
//     normals.PushBack(FTVector3(0.0f, 0.0f, -1.0f));
//     normals.PushBack(FTVector3(0.0f, 0.0f, -1.0f));
//     normals.PushBack(FTVector3(0.0f, 0.0f, -1.0f));
//     normals.PushBack(FTVector3(0.0f, 0.0f, -1.0f));
//
//     // Texture Coordinates (Direct3D 9)
//     // https://learn.microsoft.com/en-us/windows/win32/direct3d9/texture-coordinates
//     FTRectArea& rectOnMap = tile.GetRectOnMap();
//     const FTVector2& mapMin = rectOnMap.GetMin();
//     const float widthInMap = rectOnMap.GetSize().x;
//     const float heightInMap = rectOnMap.GetSize().y;
//
//     texcoords.PushBack(FTVector2(mapMin.x, mapMin.y));
//     texcoords.PushBack(FTVector2(mapMin.x + widthInMap, mapMin.y));
//     texcoords.PushBack(FTVector2(mapMin.x + widthInMap, mapMin.y + heightInMap));
//     texcoords.PushBack(FTVector2(mapMin.x, mapMin.y + heightInMap));
//
//     FTMeshData meshData;
//     for (size_t i = 0; i < positions.GetSize(); i++) {
//         Vertex v;
//         v.position = positions[i];
//         v.color = colors[i];
//         v.normal = normals[i];
//         v.texcoord = texcoords[i];
//         meshData.Vertices.PushBack(v);
//     }
//     meshData.Indices = {
//         0, 1, 2, 0, 2, 3, // 앞면
//     };
//
//     return meshData;
// }

// FTMeshData GeometryGenerator::MakeBox()
//{
//	return MakeBox(1.f);
// }

// FTMeshData GeometryGenerator::MakeBox(float scale)
//{
//	Common::FTDS::DynamicArray<FTVector3> positions;
//	Common::FTDS::DynamicArray<FTVector3> colors;
//	Common::FTDS::DynamicArray<FTVector3> normals;
//	Common::FTDS::DynamicArray<FTVector2> texcoords; // 텍스춰 좌표
//
//	// 윗면
//	positions.PushBack(FTVector3(-1.0f, 1.0f, -1.0f) * scale);
//	positions.PushBack(FTVector3(-1.0f, 1.0f, 1.0f) * scale);
//	positions.PushBack(FTVector3(1.0f, 1.0f, 1.0f) * scale);
//	positions.PushBack(FTVector3(1.0f, 1.0f, -1.0f) * scale);
//	colors.PushBack(FTVector3(1.0f, 0.0f, 0.0f));
//	colors.PushBack(FTVector3(1.0f, 0.0f, 0.0f));
//	colors.PushBack(FTVector3(1.0f, 0.0f, 0.0f));
//	colors.PushBack(FTVector3(1.0f, 0.0f, 0.0f));
//	normals.PushBack(FTVector3(0.0f, 1.0f, 0.0f));
//	normals.PushBack(FTVector3(0.0f, 1.0f, 0.0f));
//	normals.PushBack(FTVector3(0.0f, 1.0f, 0.0f));
//	normals.PushBack(FTVector3(0.0f, 1.0f, 0.0f));
//	texcoords.PushBack(FTVector2(0.0f, 0.0f));
//	texcoords.PushBack(FTVector2(1.0f, 0.0f));
//	texcoords.PushBack(FTVector2(1.0f, 1.0f));
//	texcoords.PushBack(FTVector2(0.0f, 1.0f));
//
//	// 아랫면
//	positions.PushBack(FTVector3(-1.0f, -1.0f, -1.0f) * scale);
//	positions.PushBack(FTVector3(1.0f, -1.0f, -1.0f) * scale);
//	positions.PushBack(FTVector3(1.0f, -1.0f, 1.0f) * scale);
//	positions.PushBack(FTVector3(-1.0f, -1.0f, 1.0f) * scale);
//	colors.PushBack(FTVector3(0.0f, 1.0f, 0.0f));
//	colors.PushBack(FTVector3(0.0f, 1.0f, 0.0f));
//	colors.PushBack(FTVector3(0.0f, 1.0f, 0.0f));
//	colors.PushBack(FTVector3(0.0f, 1.0f, 0.0f));
//	normals.PushBack(FTVector3(0.0f, -1.0f, 0.0f));
//	normals.PushBack(FTVector3(0.0f, -1.0f, 0.0f));
//	normals.PushBack(FTVector3(0.0f, -1.0f, 0.0f));
//	normals.PushBack(FTVector3(0.0f, -1.0f, 0.0f));
//	texcoords.PushBack(FTVector2(0.0f, 0.0f));
//	texcoords.PushBack(FTVector2(1.0f, 0.0f));
//	texcoords.PushBack(FTVector2(1.0f, 1.0f));
//	texcoords.PushBack(FTVector2(0.0f, 1.0f));
//
//	// 앞면
//	positions.PushBack(FTVector3(-1.0f, -1.0f, -1.0f) * scale);
//	positions.PushBack(FTVector3(-1.0f, 1.0f, -1.0f) * scale);
//	positions.PushBack(FTVector3(1.0f, 1.0f, -1.0f) * scale);
//	positions.PushBack(FTVector3(1.0f, -1.0f, -1.0f) * scale);
//	colors.PushBack(FTVector3(0.0f, 0.0f, 1.0f));
//	colors.PushBack(FTVector3(0.0f, 0.0f, 1.0f));
//	colors.PushBack(FTVector3(0.0f, 0.0f, 1.0f));
//	colors.PushBack(FTVector3(0.0f, 0.0f, 1.0f));
//	normals.PushBack(FTVector3(0.0f, 0.0f, -1.0f));
//	normals.PushBack(FTVector3(0.0f, 0.0f, -1.0f));
//	normals.PushBack(FTVector3(0.0f, 0.0f, -1.0f));
//	normals.PushBack(FTVector3(0.0f, 0.0f, -1.0f));
//	texcoords.PushBack(FTVector2(0.0f, 0.0f));
//	texcoords.PushBack(FTVector2(1.0f, 0.0f));
//	texcoords.PushBack(FTVector2(1.0f, 1.0f));
//	texcoords.PushBack(FTVector2(0.0f, 1.0f));
//
//	// 뒷면
//	positions.PushBack(FTVector3(-1.0f, -1.0f, 1.0f) * scale);
//	positions.PushBack(FTVector3(1.0f, -1.0f, 1.0f) * scale);
//	positions.PushBack(FTVector3(1.0f, 1.0f, 1.0f) * scale);
//	positions.PushBack(FTVector3(-1.0f, 1.0f, 1.0f) * scale);
//	colors.PushBack(FTVector3(0.0f, 1.0f, 1.0f));
//	colors.PushBack(FTVector3(0.0f, 1.0f, 1.0f));
//	colors.PushBack(FTVector3(0.0f, 1.0f, 1.0f));
//	colors.PushBack(FTVector3(0.0f, 1.0f, 1.0f));
//	normals.PushBack(FTVector3(0.0f, 0.0f, 1.0f));
//	normals.PushBack(FTVector3(0.0f, 0.0f, 1.0f));
//	normals.PushBack(FTVector3(0.0f, 0.0f, 1.0f));
//	normals.PushBack(FTVector3(0.0f, 0.0f, 1.0f));
//	texcoords.PushBack(FTVector2(0.0f, 0.0f));
//	texcoords.PushBack(FTVector2(1.0f, 0.0f));
//	texcoords.PushBack(FTVector2(1.0f, 1.0f));
//	texcoords.PushBack(FTVector2(0.0f, 1.0f));
//
//	// 왼쪽
//	positions.PushBack(FTVector3(-1.0f, -1.0f, 1.0f) * scale);
//	positions.PushBack(FTVector3(-1.0f, 1.0f, 1.0f) * scale);
//	positions.PushBack(FTVector3(-1.0f, 1.0f, -1.0f) * scale);
//	positions.PushBack(FTVector3(-1.0f, -1.0f, -1.0f) * scale);
//	colors.PushBack(FTVector3(1.0f, 1.0f, 0.0f));
//	colors.PushBack(FTVector3(1.0f, 1.0f, 0.0f));
//	colors.PushBack(FTVector3(1.0f, 1.0f, 0.0f));
//	colors.PushBack(FTVector3(1.0f, 1.0f, 0.0f));
//	normals.PushBack(FTVector3(-1.0f, 0.0f, 0.0f));
//	normals.PushBack(FTVector3(-1.0f, 0.0f, 0.0f));
//	normals.PushBack(FTVector3(-1.0f, 0.0f, 0.0f));
//	normals.PushBack(FTVector3(-1.0f, 0.0f, 0.0f));
//	texcoords.PushBack(FTVector2(0.0f, 0.0f));
//	texcoords.PushBack(FTVector2(1.0f, 0.0f));
//	texcoords.PushBack(FTVector2(1.0f, 1.0f));
//	texcoords.PushBack(FTVector2(0.0f, 1.0f));
//
//	// 오른쪽
//	positions.PushBack(FTVector3(1.0f, -1.0f, 1.0f) * scale);
//	positions.PushBack(FTVector3(1.0f, -1.0f, -1.0f) * scale);
//	positions.PushBack(FTVector3(1.0f, 1.0f, -1.0f) * scale);
//	positions.PushBack(FTVector3(1.0f, 1.0f, 1.0f) * scale);
//	colors.PushBack(FTVector3(1.0f, 0.0f, 1.0f));
//	colors.PushBack(FTVector3(1.0f, 0.0f, 1.0f));
//	colors.PushBack(FTVector3(1.0f, 0.0f, 1.0f));
//	colors.PushBack(FTVector3(1.0f, 0.0f, 1.0f));
//	normals.PushBack(FTVector3(1.0f, 0.0f, 0.0f));
//	normals.PushBack(FTVector3(1.0f, 0.0f, 0.0f));
//	normals.PushBack(FTVector3(1.0f, 0.0f, 0.0f));
//	normals.PushBack(FTVector3(1.0f, 0.0f, 0.0f));
//	texcoords.PushBack(FTVector2(0.0f, 0.0f));
//	texcoords.PushBack(FTVector2(1.0f, 0.0f));
//	texcoords.PushBack(FTVector2(1.0f, 1.0f));
//	texcoords.PushBack(FTVector2(0.0f, 1.0f));
//
//	FTMeshData meshData;
//	for (size_t i = 0; i < positions.GetSize(); i++)
//	{
//		Vertex v;
//		v.position = positions[i];
//		// v.color     = colors[i];
//		v.normal   = normals[i];
//		v.texcoord = texcoords[i];
//		meshData.Vertices.PushBack(v);
//	}
//
//	meshData.Indices = {
//		0, 1, 2, 0, 2, 3, // Up
//		4,
//		5,
//		6,
//		4,
//		6,
//		7, // Down
//		8,
//		9,
//		10,
//		8,
//		10,
//		11, // Front
//		12,
//		13,
//		14,
//		12,
//		14,
//		15, // Back
//		16,
//		17,
//		18,
//		16,
//		18,
//		19, // Left
//		20,
//		21,
//		22,
//		20,
//		22,
//		23 // Right
//	};
//
//	return meshData;
// }

// FTMeshData GeometryGenerator::MakeSquareGrid(
//	const float width,
//	const float height,
//	const int	numSlices,
//	const int	numStacks)
//{
//	FTMeshData meshData;
//
//	const float dx = width / numSlices;
//	const float dy = height / numStacks;
//
//	Common::FTDS::DynamicArray<Vertex>&   vertices = meshData.Vertices;
//	Common::FTDS::DynamicArray<uint32_t>& indices	= meshData.Indices;
//
//	FTVector3 leftBottom = FTVector3(-0.5f * width, -0.5f * height, 0.0f);
//
//	for (size_t j = 0; j <= numStacks; ++j)
//	{
//		FTVector3 startStackPoint =
//			FTVector3::Transform(leftBottom, Matrix::CreateTranslation(0.0f, dy * j, 0.0f));
//
//		for (size_t i = 0; i <= numSlices; ++i)
//		{
//			Vertex v;
//			v.position = FTVector3::Transform(startStackPoint, Matrix::CreateTranslation(dx * i, 0.0f, 0.0f));
//			v.normal   = -FTVector3::UnitZ;
//			v.texcoord = FTVector2(float(i) / numSlices, 1.0f - float(j) / numStacks);
//			vertices.PushBack(v);
//		}
//	}
//
//	for (int j = 0; j < numStacks; ++j)
//	{
//		const int offset = (numSlices + 1) * j;
//
//		for (int i = 0; i < numSlices; ++i)
//		{
//			indices.PushBack(offset + i);
//			indices.PushBack(offset + i + numSlices + 1);
//			indices.PushBack(offset + i + 1 + numSlices + 1);
//
//			indices.PushBack(offset + i);
//			indices.PushBack(offset + i + 1 + numSlices + 1);
//			indices.PushBack(offset + i + 1);
//		}
//	}
//
//	return meshData;
// }
//
// FTMeshData GeometryGenerator::MakeCylinder(const float bottomRadius, const float topRadius, float height, int sliceCount)
//{
//	using namespace DirectX;
//	using DirectX::SimpleMath::Matrix;
//	using DirectX::SimpleMath::FTVector3;
//
//	// Texture 좌표계때문에 (sliceCount + 1) x 2 개의 버텍스 사용
//
//	const float dTheta = -XM_2PI / float(sliceCount);
//
//	FTMeshData			 meshData;
//	Common::FTDS::DynamicArray<Vertex>& vertices = meshData.Vertices;
//
//	// 옆면의 바닥 버텍스들 (인덱스 0 이상 sliceCount 미만)
//	for (int i = 0; i <= sliceCount; i++)
//	{
//		Vertex v;
//
//		// TODO: 작성 (텍스춰 좌표계, 버텍스 노멀 필요)
//		v.position = FTVector3::Transform(FTVector3(bottomRadius, -0.5f * height, 0.0f), Matrix::CreateRotationY(dTheta * float(i)));
//		v.normal   = v.position - FTVector3(0.0f, -0.5f * height, 0.0f);
//		v.normal.Normalize();
//		v.texcoord = FTVector2(float(i) / sliceCount, 0.0f);
//
//		vertices.PushBack(v);
//	}
//
//	// 옆면의 맨 위 버텍스들 (인덱스 sliceCount 이상 2 * sliceCount 미만)
//	for (int i = 0; i <= sliceCount; i++)
//	{
//		Vertex v;
//
//		// TODO: 작성 (텍스춰 좌표계, 버텍스 노멀 필요)
//		v.position = FTVector3::Transform(FTVector3(bottomRadius, 0.5f * height, 0.0f), Matrix::CreateRotationY(dTheta * float(i)));
//		v.normal   = v.position - FTVector3(0.0f, 0.5f * height, 0.0f);
//		v.normal.Normalize();
//		v.texcoord = FTVector2(float(i) / sliceCount, 1.0f);
//
//		vertices.PushBack(v);
//	}
//
//	Common::FTDS::DynamicArray<uint32_t>& indices = meshData.Indices;
//
//	for (int i = 0; i < sliceCount; i++)
//	{
//		// TODO: 삼각형 두 개 씩
//		indices.PushBack(i);
//		indices.PushBack(i + sliceCount + 1);
//		indices.PushBack(i + sliceCount + 1 + 1);
//
//		indices.PushBack(i);
//		indices.PushBack(i + 1 + sliceCount + 1);
//		indices.PushBack(i + 1);
//	}
//
//	return meshData;
// }
//
// FTMeshData GeometryGenerator::MakeSphere(const float radius, const int numSlices, const int numStacks)
//{
//	using namespace DirectX;
//
//	const float dTheta = -XM_2PI / float(numSlices);
//	const float dPhi   = -XM_PI / float(numStacks);
//
//	FTMeshData meshData;
//
//	Common::FTDS::DynamicArray<Vertex>& vertices = meshData.Vertices;
//
//	for (int j = 0; j <= numStacks; j++)
//	{
//
//		FTVector3 stackStartPoint =
//			FTVector3::Transform(
//				FTVector3(0.0f, -radius, 0.0f), Matrix::CreateRotationZ(dPhi * float(j)));
//
//		for (int i = 0; i <= numSlices; i++)
//		{
//			Vertex v;
//
//			v.position = FTVector3::Transform(
//				stackStartPoint, Matrix::CreateRotationY(dTheta * float(i)));
//			v.normal = v.position;
//			v.normal.Normalize();
//			v.texcoord = FTVector2(float(i) / numSlices, 1 - float(j) / numStacks);
//
//			vertices.PushBack(v);
//		}
//	}
//
//	Common::FTDS::DynamicArray<uint32_t>& indices = meshData.Indices;
//
//	for (int j = 0; j < numStacks; j++)
//	{
//
//		const int offset = (numSlices + 1) * j;
//
//		for (int i = 0; i < numSlices; i++)
//		{
//
//			indices.PushBack(offset + i);
//			indices.PushBack(offset + i + numSlices + 1);
//			indices.PushBack(offset + i + 1 + numSlices + 1);
//
//			indices.PushBack(offset + i);
//			indices.PushBack(offset + i + 1 + numSlices + 1);
//			indices.PushBack(offset + i + 1);
//		}
//	}
//
//	return meshData;
// }

// Common::FTDS::DynamicArray<FTMeshData*> GeometryGenerator::ReadFromFile(const Common::FTDS::String& resPath)
//{
//	using namespace DirectX;
//
//	ModelLoader modelLoader;
//	modelLoader.Load(resPath);
//	Common::FTDS::DynamicArray<FTMeshData>& meshes = modelLoader.meshes;
//
//	// Normalize Vertices
//	FTVector3 vmin(1000, 1000, 1000);
//	FTVector3 vmax(-1000, -1000, -1000);
//	for (auto& mesh : meshes)
//	{
//		mesh.Vertices.IterateArray([&](Vertex& v) {
//			vmin.x = XMMin(vmin.x, v.position.x);
//			vmin.y = XMMin(vmin.y, v.position.y);
//			vmin.z = XMMin(vmin.z, v.position.z);
//			vmax.x = XMMax(vmax.x, v.position.x);
//			vmax.y = XMMax(vmax.y, v.position.y);
//			vmax.z = XMMax(vmax.z, v.position.z);
//		});
//	}
//
//	float dx = vmax.x - vmin.x, dy = vmax.y - vmin.y, dz = vmax.z - vmin.z;
//	float dl = XMMax(XMMax(dx, dy), dz);
//	float cx = (vmax.x + vmin.x) * 0.5f, cy = (vmax.y + vmin.y) * 0.5f,
//		  cz = (vmax.z + vmin.z) * 0.5f;
//
//	for (auto& mesh : meshes)
//	{
//		mesh.Vertices.IterateArray([&](Vertex& v) {
//			v.position.x = (v.position.x - cx) / dl;
//			v.position.y = (v.position.y - cy) / dl;
//			v.position.z = (v.position.z - cz) / dl;
//		});
//	}
//
//	Common::FTDS::DynamicArray<FTMeshData*> meshData;
//	meshData.Reserve(meshes.GetSize());
//
//	for (FTMeshData meshD : meshes)
//		meshData.PushBack(&meshD);
//
//	return meshData;
// }

// Common::FTDS::DynamicArray<FTMeshData> GeometryGenerator::ReadFromFile(Common::FTDS::String& basePath, Common::FTDS::String& filename)
//{
//	using namespace DirectX;
//
//	ModelLoader modelLoader;
//	modelLoader.Load(basePath, filename);
//	Common::FTDS::DynamicArray<FTMeshData>& meshes = modelLoader.meshes;
//
//	// Normalize Vertices
//	FTVector3 vmin(1000, 1000, 1000);
//	FTVector3 vmax(-1000, -1000, -1000);
//	for (auto& mesh : meshes)
//	{
//		for (auto& v : mesh.Vertices)
//		{
//			vmin.x = XMMin(vmin.x, v.position.x);
//			vmin.y = XMMin(vmin.y, v.position.y);
//			vmin.z = XMMin(vmin.z, v.position.z);
//			vmax.x = XMMax(vmax.x, v.position.x);
//			vmax.y = XMMax(vmax.y, v.position.y);
//			vmax.z = XMMax(vmax.z, v.position.z);
//		}
//	}
//
//	float dx = vmax.x - vmin.x, dy = vmax.y - vmin.y, dz = vmax.z - vmin.z;
//	float dl = XMMax(XMMax(dx, dy), dz);
//	float cx = (vmax.x + vmin.x) * 0.5f, cy = (vmax.y + vmin.y) * 0.5f,
//		  cz = (vmax.z + vmin.z) * 0.5f;
//
//	for (auto& mesh : meshes)
//	{
//		for (auto& v : mesh.Vertices)
//		{
//			v.position.x = (v.position.x - cx) / dl;
//			v.position.y = (v.position.y - cy) / dl;
//			v.position.z = (v.position.z - cz) / dl;
//		}
//	}
//
//	return meshes;
// }
