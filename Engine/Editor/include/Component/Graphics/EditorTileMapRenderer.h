// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// This renders FTTilemaps using FTTexture as a texture pack,
/// primitive square as a mesh, and FTTileMap as a .CSV source.
/// </summary>

#pragma once
#include "Component/IEditorComponent.h"
#include "Component/TileMapRenderer.h"

namespace Editor
{
	class EditorTileMapRenderer :
		public IEditorComponent<D3D11::TileMapRenderer>
	{

	};
} // namespace Editor