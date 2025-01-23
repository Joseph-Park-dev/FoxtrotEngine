// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "SpriteRenderer.h"

#include "Actors/Actor.h"
#include "Actors/Transform.h"
#include "Core/FTCore.h"
#include "FileSystem/ChunkLoader.h"
#include "FileSystem/ChunkFileKeys.h"
#include "FileSystem/FileIOHelper.h"
#include "Managers/ResourceManager.h"
#include "Math/FTMath.h"
#include "Renderer/Camera.h"
#include "Renderer/D3D11Utils.h"
#include "Renderer/FoxtrotRenderer.h"
#include "ResourceSystem/FTBasicMeshGroup.h"
#include "ResourceSystem/FTTexture.h"
#include "ResourceSystem/GeometryGenerator.h"
#include "ResourceSystem/Mesh.h"
#include "ResourceSystem/MeshData.h"
#include "Scenes/Scene.h"

#ifdef FOXTROT_EDITOR
#include "CommandHistory.h"
#include "EditorUtils.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/FileDialog/ImGuiFileDialog.h"
#include "imgui/FileDialog/ImGuiFileDialogConfig.h"
#include <imgui.h>
#endif // FOXTROT_EDITOR

void SpriteRenderer::Initialize(FTCore* coreInstance)
{
	SetRenderer	(coreInstance->GetGameRenderer());

	if(GetOwner()->GetActorGroup() == ActorGroup::PLAYER)
		SetMeshKey	(ChunkKeys::PRIMITIVE_SQUARE_BLUE);
	else if (GetOwner()->GetActorGroup() == ActorGroup::ENEMY)
		SetMeshKey(ChunkKeys::PRIMITIVE_SQUARE_RED);
	else
		SetMeshKey(ChunkKeys::PRIMITIVE_SQUARE_GREEN);

	if (GetMeshKey() != ChunkKeys::VALUE_NOT_ASSIGNED)
	{
		this->InitializeMesh();
		if (GetTexKey() != ChunkKeys::VALUE_NOT_ASSIGNED)
			MeshRenderer::SetTexture();
	}
	Component::Initialize(coreInstance);
}

void SpriteRenderer::CloneTo(Actor* actor)
{
	SpriteRenderer* newComp = DBG_NEW SpriteRenderer(actor, GetUpdateOrder());
	newComp->SetMeshKey(this->GetMeshKey());
	newComp->SetTexKey(this->GetTexKey());
	newComp->mChannel = this->mChannel;
	newComp->mTexScale = this->mTexScale;
}

bool SpriteRenderer::InitializeMesh()
{
	MeshRenderer::InitializeMesh(
		ResourceManager::GetInstance()->GetLoadedPrimitive(
			GetMeshKey()
		)
	);
	if (!GetMeshGroup())
	{
		LogString("ERROR: SpriteRenderer::InitializeMesh() -> Mesh "
			"Init failed");
		return false;
	}
	return true;
}

SpriteRenderer::SpriteRenderer(Actor* owner,int updateOrder)
	: MeshRenderer(owner, updateOrder)
	, mChannel(4)
	, mTexScale(FTVector2(1.0f,1.0f)) 
{}

void SpriteRenderer::SaveProperties(std::ofstream& ofs)
{
	Component::SaveProperties(ofs);
	FileIOHelper::SaveVector2(ofs, ChunkKeys::TEXTURE_WIDTH, mTexScale);
	FileIOHelper::SaveUnsignedInt(ofs, ChunkKeys::TEXTURE_KEY, GetTexKey());
}

void SpriteRenderer::LoadProperties(std::ifstream& ifs)
{
	UINT texKey = 0;
	FileIOHelper::LoadUnsignedInt(ifs, texKey);
	SetTexKey(texKey);
	FileIOHelper::LoadVector2(ifs, mTexScale);
	Component::LoadProperties(ifs);
}

#ifdef FOXTROT_EDITOR
void SpriteRenderer::EditorUpdate(float deltaTime)
{
	Update(deltaTime);
}

void SpriteRenderer::EditorRender(FoxtrotRenderer* renderer)
{
	//SpriteRenderer::Render(renderer);
}

void SpriteRenderer::EditorUIUpdate()
{
	SetRenderer(FTCoreEditor::GetInstance()->GetGameRenderer());
	CHECK_RENDERER(GetRenderer());
	OnConfirmUpdate();
	UpdateSprite();
	OnResetTexture();
}

void SpriteRenderer::OnConfirmUpdate()
{
	if (ImGui::Button("Update"))
		this->SetTexture();
}

void SpriteRenderer::OnResetTexture()
{
	if (ImGui::Button("Reset"))
	{
		GetTexture()->ReleaseTexture();
		SetTexKey(ChunkKeys::VALUE_NOT_ASSIGNED);
	}
}

void SpriteRenderer::UpdateSprite()
{
	std::string currentSprite = "No sprite has been assigned";
	if (GetTexKey() != ChunkKeys::VALUE_NOT_ASSIGNED)
	{
		currentSprite =
			"Current sprite : \n" + ResourceManager::GetInstance()->GetLoadedTexture(GetTexKey())->GetRelativePath();
		if (GetMeshGroup() && GetTexture())
		{
			ImVec2 size = ImVec2(100, 100);
			ImGui::Image((ImTextureID)GetTexture()->GetResourceView().Get(), size);
		}
	}
	ImGui::Text(currentSprite.c_str());

	UINT key = 
		FTEditorUtils::DisplayResSelection<FTTexture>(
			"Select Sprite", 
			ResourceManager::GetInstance()->GetTexturesMap()
		);
	if (key != ChunkKeys::VALUE_NOT_ASSIGNED)
		SetTexKey(key);
}

void SpriteRenderer::UpdateSprite(UINT& key)
{
	std::string currentSprite = {};
	if (key != ChunkKeys::VALUE_NOT_ASSIGNED)
	{
		FTTexture* sprite = ResourceManager::GetInstance()->GetLoadedTexture(key);
		currentSprite =
			"Current sprite : \n" + sprite->GetRelativePath();
		if (sprite)
		{
			ImVec2 size = ImVec2(100, 100);
			ImGui::Image((ImTextureID)sprite->GetResourceView().Get(), size);
		}
	}
	else
		currentSprite = "No sprite has been assigned";

	ImGui::Text(currentSprite.c_str());

	if (ImGui::Button("Select Sprite"))
	{
		IGFD::FileDialogConfig config;
		config.path = ".";
		config.countSelectionMax = 1;
		ImGuiFileDialog::Instance()->OpenDialog(
			"SelectSprite", "Select Sprite", ChunkKeys::TEXTURE_FORMAT_SUPPORTED, config);
		ImGui::OpenPopup("Select Sprite");
	}

	if (ImGui::BeginPopupModal("Select Sprite", NULL,
		ImGuiWindowFlags_MenuBar))
	{
		std::unordered_map<UINT, FTTexture*>& texturesMap =
			ResourceManager::GetInstance()->GetTexturesMap();
		if (ImGui::TreeNode("Selection State: Single Selection"))
		{
			UINT	   spriteKey = ChunkKeys::VALUE_NOT_ASSIGNED;
			static int selected = -1;
			int		   i = 0;
			for (auto iter = texturesMap.begin(); iter != texturesMap.end();
				++iter, ++i)
			{
				if (ImGui::Selectable((*iter).second->GetFileName().c_str(),
					selected == i))
				{
					spriteKey = (*iter).first;
					selected = i;
				}
			}
			ImGui::TreePop();
			if (selected != -1)
			{
				key = spriteKey;
			}
		}
		if (ImGui::Button("Close"))
			ImGui::CloseCurrentPopup();
		ImGui::EndPopup();
	}
}
#endif // FOXTROT_EDITOR