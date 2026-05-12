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

#ifdef FOXTROT_EDITOR
	#include "Utility/EditorHelper.h"
#endif // FOXTROT_EDITOR
	

namespace D3D11
{
	// Debug shape only takes a point in VS,
	// and utilizes that in GS to form a geometry
	struct DebugVCData
	{
		Math::FTMatrix4 model = Math::FTMatrix4();
	};

	static_assert((sizeof(DebugVCData) % 16) == 0, "Constant Buffer size must be 16-byte aligned");

	struct DebugGCData
	{
		Math::FTMatrix4 view	   = Math::FTMatrix4();
		Math::FTMatrix4 projection = Math::FTMatrix4();

		Math::FTVector2 size;
		Math::FTVector2 dummy;
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
		Math::FTVector4 IndexColor;
		uint32_t		IsActive;
		uint32_t		dummy[3];
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
		Math::FTMatrix4 ModelMat = Math::FTMatrix4();
	};
	static_assert((sizeof(PointModelMat) % 16) == 0, "Constant Buffer size must be 16-byte aligned");

	struct PointVPMat
	{
		Math::FTMatrix4 ViewMat = Math::FTMatrix4();
		Math::FTMatrix4 ProjMat = Math::FTMatrix4();
	};
	static_assert((sizeof(PointVPMat) % 16) == 0, "Constant Buffer size must be 16-byte aligned");

	//////////////////////////////////
	//////// Sprite Animation ////////
	//////////////////////////////////

	struct SpriteGCData
	{
		Math::FTVector2 Size  = Math::FTVector2(1.f, 1.f);
		Math::FTVector2 Scale = Math::FTVector2(1.f, 1.f);
		Math::FTVector4 Frame = Math::FTVector4(0.f, 0.f, 1.f, 1.f);
		Math::FTVector2 Pivot = Math::FTVector2(0.5f, 0.5f);
		Math::FTVector2 Dummy;

#ifdef FOXTROT_EDITOR
		void UpdateUI()
		{
			Editor::UPDATE_VEC2("Sprite Size", Size);
		}
#endif // FOXTROT_EDITOR
	};

	static_assert((sizeof(SpriteGCData) % 16) == 0, "Constant Buffer size must be 16-byte aligned");

	struct SpritePSData
	{
		Math::FTVector4 Color;
		bool			UseTexture;
		Math::FTVector3 Dummy;
	};
	static_assert((sizeof(SpritePSData) % 16) == 0, "Constant Buffer size must be 16-byte aligned");
} // namespace D3D11