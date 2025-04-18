// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// File types (extensions) that are used in Foxtrot Engine.
/// </summary>

#pragma once
namespace FileTypes
{
	// Essential Files
	constexpr const char* CHUNK	 = ".chunk";
	constexpr const char* GDPACK = ".gdpack";

	// FTResources
	constexpr const char* PREMADE		   = ".ftpremade";
	constexpr const char* TEXTURE		   = ".png, .jpeg, .jpg, .dds";
	constexpr const char* DDS_TEXTURE	   = ".dds";
	constexpr const char* TILEMAP		   = ".fttilemap";
	constexpr const char* SPRITE_SHEET	   = ".ftspritesheet";
	constexpr const char* CSV			   = ".csv";
	constexpr const char* JSON			   = ".json";
	constexpr const char* MESH			   = ".fbx, .stl";
	constexpr const char* SPRITE_ANIMATION = ".ftspriteanim";
	constexpr const char* SHADER		   = ".hlsl";
	constexpr const char* MATERIAL		   = ".ftmat";

	constexpr const char* VERTEX_SHADER = "VS";
	constexpr const char* PIXEL_SHADER	= "PS";
} // namespace FileTypes