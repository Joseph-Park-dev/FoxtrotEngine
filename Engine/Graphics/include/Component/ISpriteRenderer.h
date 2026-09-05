// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Base Component for rendering 2D sprites.
/// This uses FTTexture as an image source, and primitive square
/// as a mesh to display FTTexture on.
/// </summary>

#pragma once
#include "IComponent.h"

namespace Graphics
{
	class ISprite;
	class IMaterial;
	class IPSO;

	class ISpriteRenderer :
		public Common::IComponent
	{
	};

	namespace ChunkKey
	{
		namespace SpriteRenderer
		{
			constexpr const char* SPRITE	   = "Sprite";
			constexpr const char* MATERIAL	   = "Material";
			constexpr const char* FRONT_DIR	   = "Front Dir";
			constexpr const char* SPRITE_SCALE = "Scale";
			constexpr const char* PSO		   = "SpritePSO";
		} // namespace SpriteRenderer
	} // namespace ChunkKey
} // namespace Graphics