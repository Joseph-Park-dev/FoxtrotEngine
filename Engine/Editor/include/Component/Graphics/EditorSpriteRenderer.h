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
#include "Component/IEditorComponent.h"
#include "Component/SpriteRenderer.h"

namespace Editor
{
	class EditorSpriteRenderer :
		public IEditorComponent<D3D11::SpriteRenderer>
	{

	};
} // namespace Editor