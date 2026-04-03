// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Constant Data for the Meshes in DirectX 11 context.
/// </summary>

#pragma once
#include "Math/FTMath.h"

namespace D3D11
{
	// Debug shape only takes a point in VS,
	// and utilizes that in GS to form a geometry
	struct DebugVCData
	{
		FTMatrix4 model = FTMatrix4();
	};

	static_assert((sizeof(DebugVCData) % 16) == 0, "Constant Buffer size must be 16-byte aligned");

	struct DebugGCData
	{
		FTMatrix4 view		 = FTMatrix4();
		FTMatrix4 projection = FTMatrix4();

		FTVector2 size;
		FTVector2 dummy;
	};

	static_assert((sizeof(DebugGCData) % 16) == 0, "Constant Buffer size must be 16-byte aligned");

	struct NormalVCData
	{
		float scale = 0.1f;
		float dummy[3];
	};
	static_assert((sizeof(NormalVCData) % 16) == 0, "Constant Buffer size must be 16-byte aligned");

	struct TileMapConstantData
	{
		float texCoordX;
		float texCoordY;
		float texSizeX;
		float texSizeY;
	};
	static_assert((sizeof(TileMapConstantData) % 16) == 0, "Constant Buffer size must be 16-byte aligned");

	// Data use for Mouse Picking
	struct IndexPCData
	{
		FTVector4 IndexColor;
		uint32_t  IsActive;
		uint32_t  dummy[3];
	};

	struct DebugPCData
	{
		uint32_t IsActive;
		uint32_t dummy[3];
	};

	//////////////////////////////////////
	//////// Basic Mesh Rendering ////////
	//////////////////////////////////////
	struct PointModelMat
	{
		FTMatrix4 ModelMat = FTMatrix4();
	};
	static_assert((sizeof(PointModelMat) % 16) == 0, "Constant Buffer size must be 16-byte aligned");

	struct PointVPMat
	{
		FTMatrix4 ViewMat = FTMatrix4();
		FTMatrix4 ProjMat = FTMatrix4();
	};
	static_assert((sizeof(PointVPMat) % 16) == 0, "Constant Buffer size must be 16-byte aligned");

	//////////////////////////////////
	//////// Sprite Animation ////////
	//////////////////////////////////

	struct SpriteGCData
	{
		FTVector2 Size	= FTVector2(1.f, 1.f);
		FTVector2 Scale = FTVector2(1.f, 1.f);
		FTVector4 Frame = FTVector4(0.f, 0.f, 1.f, 1.f);
		FTVector2 Pivot = FTVector2(0.5f, 0.5f);
		FTVector2 Dummy;

#ifdef FOXTROT_EDITOR
		void UpdateUI()
		{
			CommandHistory::GetInstance()->UpdateVector2Value("Sprite Size", Size);
		}
#endif // FOXTROT_EDITOR
	};

	static_assert((sizeof(SpriteGCData) % 16) == 0, "Constant Buffer size must be 16-byte aligned");

	struct SpritePSData
	{
		FTVector4 Color;
		bool	  UseTexture;
		FTVector3 Dummy;
	};
	static_assert((sizeof(SpritePSData) % 16) == 0, "Constant Buffer size must be 16-byte aligned");
} // namespace D3D11