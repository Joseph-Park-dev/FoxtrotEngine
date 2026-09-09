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
#include "FTDS/Dynamic/DynamicArray.h"
#include "ResourceSystem/Mesh/FTMeshData.h"

namespace D3D11
{
	using namespace Common;
	class Tile;
	class FTTileMap;
	struct TextAttribute;
	struct FTDebugMeshData;

	class GeometryGenerator
	{
	public:
		/// @brief Builds mesh data representing a square.
		/// @return Created square instance or resource.
		static FTMeshData	   MakeSquare();
		/// @brief Builds mesh data representing a square.
		/// @param scale Scale factor applied to the content.
		/// @return Created square instance or resource.
		static FTMeshData	   MakeSquare(float scale);
		/// @brief Builds mesh data representing a point.
		/// @param color Color components used when rendering.
		/// @return Created point instance or resource.
		static FTMeshData	   MakePoint(Math::FTVector4 color);
		/// @brief Creates point geometry for editor or debug visualization.
		/// @param color Color components used when rendering.
		/// @return Created debug point instance or resource.
		static FTDebugMeshData MakeDebugPoint(Math::FTVector4 color);
		/// @brief Builds mesh data representing a square.
		/// @param scale Scale factor applied to the content.
		/// @param color Color components used when rendering.
		/// @return Created square instance or resource.
		static FTMeshData*	   MakeSquare(float scale, Math::FTVector4 color);

		// MakeSquare() for Making tilemap
		/// @brief Builds the grid geometry used to render a tile map.
		/// @param tileMap Tile-map resource used for rendering.
		/// @return Created tile map grid instance or resource.
		static Common::FTDS::DynamicArray<FTMeshData> MakeTileMapGrid(FTTileMap* tileMap);

		/// @brief Builds geometry and texture coordinates for one tile.
		/// @param tile Tile instance to process.
		/// @return Created tile instance or resource.
		static FTMeshData  MakeTile(Tile& tile);
		/// @brief Creates a sprite animation from the supplied frame descriptions.
		/// @param animMeshes Meshes associated with the animation frames.
		/// @param tileMap Tile-map resource used for rendering.
		/// @param startIdx Beginning index of the selected range.
		/// @param endIdx Ending index of the selected range.
		static void		   MakeSpriteAnimation(Common::FTDS::DynamicArray<FTMeshData*>& animMeshes, Tile* tileMap, size_t startIdx, size_t endIdx);
		/// @brief Creates a sprite animation from the supplied frame descriptions.
		/// @param animMeshes Meshes associated with the animation frames.
		/// @param tileMap Tile-map resource used for rendering.
		/// @param count Number of entries to process.
		static void		   MakeSpriteAnimation(Common::FTDS::DynamicArray<FTMeshData*>& animMeshes, Tile* tileMap, size_t count);
		/// @brief Creates a frame description for an animation clip.
		/// @param tile Tile instance to process.
		/// @return Created animation frame instance or resource.
		static FTMeshData* MakeAnimationFrame(Tile& tile);
		// static void MakeSpriteTextGrid(std::vector<FTMeshData>& textMeshes, Tile* tileMap, size_t length, TextAttribute* attribute);
		// static FTMeshData MakeSpriteText(Tile tile, TextAttribute* attrib, size_t col, size_t row);

		// Make 3D Models
		// static FTMeshData MakeBox();
		// static FTMeshData MakeBox(float scale);
		// static FTMeshData MakeSquareGrid(const float width, const float height, const int numSlices, const int numStacks);
		// static FTMeshData MakeCylinder(const float bottomRadius, const float topRadius, float height, int sliceCount);
		// static FTMeshData MakeSphere(const float radius, const int numSlices, const int numStacks);

		// static FTDS::DynamicArray<FTMeshData*> ReadFromFile(const FTDS::String& resPath);
		//  static std::vector<FTMeshData> ReadFromFile(FTDS::String& basePath, FTDS::String& filename);
	};
} // namespace D3D11
