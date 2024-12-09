#include "SpriteRendererComponent.h"

#include "Actors/Actor.h"
#include "Actors/Transform.h"
#include "Core/FTCore.h"
#include "FileSystem/ChunkLoader.h"
#include "FileSystem/ChunkFileKeys.h"
#include "FileSystem/FileIOHelper.h"
#include "Managers/ResourceManager.h"
#include "Math/FTMath.h"
#include "Physics/Bounds.h"
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

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/FileDialog/ImGuiFileDialog.h"
#include "imgui/FileDialog/ImGuiFileDialogConfig.h"
#include <imgui.h>
#endif // FOXTROT_EDITOR

void SpriteRendererComponent::Initialize(FTCore* coreInstance)
{
	SetRenderer(coreInstance->GetGameRenderer());
	// Sprite Renderer uses square mesh directly - mesh key is not needed
	InitializeMesh();
	SetTexture();
	Component::Initialize(coreInstance);
}

void SpriteRendererComponent::Update(float deltaTime)
{
	if (GetMeshGroup())
		MeshRendererComponent::Update(deltaTime);
}

void SpriteRendererComponent::Render(FoxtrotRenderer* renderer)
{
	MeshRendererComponent::Render(renderer);
}

bool SpriteRendererComponent::InitializeMesh()
{
	MeshRendererComponent::InitializeMesh(
		ResourceManager::GetInstance()->GetLoadedPrimitive(
			ChunkKeys::PRIMITIVE_SQUARE
		)
	);
	if (!GetMeshGroup())
	{
		LogString("ERROR: SpriteRendererComponent::InitializeMesh() -> Mesh "
				  "Init failed");
		return false;
	}
	return true;
}

SpriteRendererComponent::SpriteRendererComponent(Actor* owner, int drawOrder,int updateOrder)
	: MeshRendererComponent(owner, drawOrder, updateOrder), mChannel(4), mTexScale(FTVector2(1.0f,1.0f)) 
{}

void SpriteRendererComponent::SaveProperties(std::ofstream& ofs)
{
	Component::SaveProperties(ofs);
	FileIOHelper::SaveVector2(ofs, ChunkKeys::TEXTURE_WIDTH, mTexScale);
	FileIOHelper::SaveUnsignedInt(ofs, ChunkKeys::TEXTURE_KEY, GetTexKey());
}

void SpriteRendererComponent::LoadProperties(std::ifstream& ifs)
{
	UINT texKey = 0;
	FileIOHelper::LoadUnsignedInt(ifs, texKey);
	SetTexKey(texKey);
	FileIOHelper::LoadVector2(ifs, mTexScale);
	Component::LoadProperties(ifs);
}

#ifdef FOXTROT_EDITOR
void SpriteRendererComponent::EditorUpdate(float deltaTime)
{
	if (GetMeshGroup())
		MeshRendererComponent::Update(deltaTime);
}

void SpriteRendererComponent::EditorRender(FoxtrotRenderer* renderer)
{
	//SpriteRendererComponent::Render(renderer);
}

void SpriteRendererComponent::EditorUIUpdate()
{
	CHECK_RENDERER(GetRenderer());
	UpdateSprite();
	ImGui::SeparatorText("Sprite Size");
	UpdateScale();
	OnConfirmUpdate();
}

void SpriteRendererComponent::OnConfirmUpdate()
{
	if (ImGui::Button("Update"))
		this->SetTexture();
}

void SpriteRendererComponent::UpdateSprite()
{
	std::string currentSprite = "No sprite has been assigned";
	if (GetTexKey() != VALUE_NOT_ASSIGNED)
	{
		currentSprite =
			"Current sprite : \n" + ResourceManager::GetInstance()->GetLoadedTexture(GetTexKey())->GetRelativePath();
		if (GetTexture())
		{
			ImVec2 size = ImVec2(100, 100);
			ImGui::Image((ImTextureID)GetTexture()->GetResourceView().Get(), size);
		}
	}
		
	ImGui::Text(currentSprite.c_str());

	if (ImGui::Button("Select Sprite"))
	{
		IGFD::FileDialogConfig config;
		config.path = ".";
		config.countSelectionMax = 1;
		ImGuiFileDialog::Instance()->OpenDialog(
			"SelectSprite", "Select Sprite", TEXTURE_FORMAT_SUPPORTED, config);
		ImGui::OpenPopup("Select Sprite");
	}

	if (ImGui::BeginPopupModal("Select Sprite", NULL,
			ImGuiWindowFlags_MenuBar))
	{
		std::unordered_map<UINT, FTTexture*>& texturesMap =
			ResourceManager::GetInstance()->GetTexturesMap();
		if (ImGui::TreeNode("Selection State: Single Selection"))
		{
			UINT	   spriteKey = VALUE_NOT_ASSIGNED;
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
				SetTexKey(spriteKey);
				//SetTexture();
			}
		}
		if (ImGui::Button("Close"))
			ImGui::CloseCurrentPopup();
		ImGui::EndPopup();
	}
}

void SpriteRendererComponent::UpdateSprite(UINT& key)
{
	std::string currentSprite = {};
	if (key != VALUE_NOT_ASSIGNED)
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
			"SelectSprite", "Select Sprite", TEXTURE_FORMAT_SUPPORTED, config);
		ImGui::OpenPopup("Select Sprite");
	}

	if (ImGui::BeginPopupModal("Select Sprite", NULL,
		ImGuiWindowFlags_MenuBar))
	{
		std::unordered_map<UINT, FTTexture*>& texturesMap =
			ResourceManager::GetInstance()->GetTexturesMap();
		if (ImGui::TreeNode("Selection State: Single Selection"))
		{
			UINT	   spriteKey = VALUE_NOT_ASSIGNED;
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

void SpriteRendererComponent::UpdateTexWidth()
{
	// CommandHistory::GetInstance()->UpdateIntValue("Texture Width",
	// &GetMeshArray()[0]->texture->GetTexWidthRef(), INTMOD_SPEED);
}

void SpriteRendererComponent::UpdateTexHeight()
{
	// CommandHistory::GetInstance()->UpdateIntValue("Texture Height",
	// &GetMeshArray()[0]->texture->GetTexHeightRef(), INTMOD_SPEED);
}

void SpriteRendererComponent::UpdateScale()
{
	CommandHistory::GetInstance()->UpdateVector2Value("Texture Scale", mTexScale,
		FLOATMOD_SPEED);
}

void SpriteRendererComponent::CloneTo(Actor* actor)
{
	SpriteRendererComponent* newComp = DBG_NEW SpriteRendererComponent(actor, GetDrawOrder(), GetUpdateOrder());
	newComp->SetMeshKey(this->GetMeshKey());
	newComp->SetTexKey(this->GetTexKey());
	newComp->mChannel = this->mChannel;
	newComp->mTexScale = this->mTexScale;
}
#endif // FOXTROT_EDITOR