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
		public IEditorComponent,
		public D3D11::TileMapRenderer
	{
	public:
		virtual void EditorUpdate(float deltaTime) override;
		virtual void EditorRender(Core::IRenderer* renderer, Core::ICamera* camInst) override;
		virtual void EditorUIUpdate() override;

	protected:
		void UpdateCSV();
		void UpdateCSV(Common::FTDS::String& key);
	};
} // namespace Editor