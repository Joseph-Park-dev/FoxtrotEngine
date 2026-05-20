// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Component/EditorMeshRenderer.h"

#include "Actor/Transform.h"
#include "Actor/Actor.h"
#include "ResourceSystem/Mesh/Mesh.h"
#include "ResourceSystem/Mesh/FTMeshDataPack.h"
#include "ResourceSystem/Mesh/FTMeshGroup.h"
#include "ResourceSystem/Mesh/GeometryGenerator.h"
#include "ResourceSystem/Material/FTMaterial.h"
#include "ResourceSystem/Shader/FTVertexShader.h"
#include "ResourceSystem/Shader/FTPixelShader.h"
#include "ResourceSystem/FTTexture.h"
#include "Renderer/Camera.h"
#include "TemplateFunctions.h"
#include "Manager/ResourceManager.h"
#include "FileSystem/ChunkLoader.h"
#include "FileSystem/FileIOHelper.h"
// #include "Compare/StringEqual.h" // NULLPATH: StringEqual.h not found in any include directory

#ifdef FOXTROT_EDITOR
	#define IMGUI_DEFINE_MATH_OPERATORS
	#include "EditorUtils.h"
	#include "EditorCamera.h"
	#include <../D3D11/include/Manager/ResourceManager.h>
#endif // FOXTROT_EDITOR

namespace Editor
{
	void EditorMeshRenderer::EditorUpdate(float deltaTime)
	{
	}

	void EditorMeshRenderer::EditorRender(Core::IRenderer* renderer, Core::ICamera* camInst)
	{
		if (GetMeshGroup())
		{
			Core::Transform* transform = GetOwner()->GetTransform();
			// mMeshGroup->Render(renderer, transform, EditorCamera::GetInstance(), mTexture, mVS, mPS, mMaterial);
		}
	}

	void EditorMeshRenderer::EditorUIUpdate()
	{
		CommandHistory::GetInstance()->UpdateBoolValue("Is Active", IsActive());

		if (!GetMeshGroup())
			return;

		// mMeshGroup->UpdateUI();

		if (GetTexture())
			GetTexture()->UpdateUI();
		// FTEditorUtils::DisplayResSelection(
		//	"Select Texture",
		//	ResourceManager::GetInstance()->GetSprites(),
		//	mTexture);

		if (GetMaterial())
			GetMaterial()->UpdateUI();

		Editor::DisplayResSelection(
			"Select Material",
			D3D11::ResourceManager::GetInstance()->GetMaterials(),
			Material());
	}
} // namespace Editor