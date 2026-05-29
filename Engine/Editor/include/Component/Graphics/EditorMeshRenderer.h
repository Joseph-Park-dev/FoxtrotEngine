// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Component related to render sprites, tilemaps, animations as meshes.
/// </summary>

#pragma once
#include "Component/IEditorComponent.h"
#include "Component/MeshRenderer.h"

#include "FTDS/Static/FTString.h"

namespace Editor
{
	class EditorMeshRenderer :
		public IEditorComponent <D3D11::MeshRenderer>
	{

} // namespace Editor