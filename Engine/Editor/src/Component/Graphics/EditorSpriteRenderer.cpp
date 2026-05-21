// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Component/Graphics/EditorSpriteRenderer.h"

#include "CommandHistory.h"
#include "EditorUtils.h"
#include "EditorCamera.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui/imgui.h>
#include <ImGuiFileDialog/ImGuiFileDialog.h>

#include "ResourceSystem/Sprite/FTSprite.h"
#include "ResourceSystem/Material/FTMaterial.h"
#include <../D3D11/include/Manager/ResourceManager.h>

namespace Editor
{
	void EditorSpriteRenderer::EditorUIUpdate()
	{
		CommandHistory::GetInstance()->UpdateBoolValue("Is Active", IsActive());

		if (GetSprite())
			GetSprite()->UpdateUI();

		Editor::DisplayResSelection(
			"Select Sprite",
			D3D11::ResourceManager::GetInstance()->GetSprites(),
			Sprite());

		if (GetMaterial())
			GetMaterial()->UpdateUI();

		Editor::DisplayResSelection(
			"Select Material",
			D3D11::ResourceManager::GetInstance()->GetMaterials(),
			Material());
	}

	void EditorSpriteRenderer::EditorUpdate(float deltaTime)
	{
	}

	void EditorSpriteRenderer::EditorRender(Core::IRenderer* renderer, Core::ICamera* camInst)
	{
		if (GetSprite())
		{
			GetSprite()->UpdateConstantBuffers(renderer, GetOwner()->GetTransform(), camInst, GetMaterial());
			GetSprite()->Render(renderer, GetOwner()->GetTransform(), camInst, GetPSO(), GetMaterial());
		}
	}
} // namespace Editor