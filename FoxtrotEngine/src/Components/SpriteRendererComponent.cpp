#include "SpriteRendererComponent.h"

#include "Actors/Actor.h"
#include "Actors/Transform.h"
#include "Core/FTCore.h"
#include "FileSystem/ChunkLoader.h"
#include "FileSystem/ChunkFileKeys.h"
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

void SpriteRendererComponent::SetTexture()
{
	MeshRendererComponent::SetTexture();
	//mTexWidth = GetMeshGroup()->GetTexture()->GetTexWidth();
	//mTexHeight = GetMeshGroup()->GetTexture()->GetTexHeight();
}

void SpriteRendererComponent::UpdateTexture(FoxtrotRenderer* renderer, std::string fileName)
{
	// std::vector<Mesh**>& meshes = renderer->GetRenderPool();
	// std::vector<Mesh**>::iterator iter = std::find(meshes.begin(),
	// meshes.end(), GetMeshArray()); if (iter != meshes.end())
	//{
	//	delete (*iter);
	//	meshes.erase(iter);

	//	//FTTexture* texture = (*iter)->texture;
	//	//ResourceManager::GetInstance()->UpdateTexture(renderer, texture,
	// channels);
	//	//mMesh->texture =
	// ResourceManager::GetInstance()->GetLoadedTexture(fileName);
	//}
}

void SpriteRendererComponent::Initialize(FTCore* coreInstance)
{
	MeshRendererComponent::Initialize(coreInstance);
	InitializeMesh();
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

SpriteRendererComponent::SpriteRendererComponent(Actor* owner, int drawOrder,
	int updateOrder)
	: MeshRendererComponent(owner, drawOrder, updateOrder), mChannel(4), mTexScale(FTVector2(1.0f,1.0f)) {}

SpriteRendererComponent::~SpriteRendererComponent() 
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
	
	SetTexture();
}

#ifdef FOXTROT_EDITOR
#include "CommandHistory.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/FileDialog/ImGuiFileDialog.h"
#include "imgui/FileDialog/ImGuiFileDialogConfig.h"
#include <imgui.h>

void SpriteRendererComponent::SaveProperties(nlohmann::ordered_json& out)
{
	Component::SaveProperties(out);
	//FileIOHelper::AddScalarValue(out["TextureWidth"], mTexWidth);
	//FileIOHelper::AddScalarValue(out["TextureHeight"], mTexHeight);
	FileIOHelper::AddScalarValue(out["Channel"], mChannel);
	FileIOHelper::AddVector2(out["Scale"], mTexScale);
}

void SpriteRendererComponent::EditorUpdate(float deltaTime)
{
	if (GetMeshGroup())
		MeshRendererComponent::Update(deltaTime);
}

void SpriteRendererComponent::EditorRender(FoxtrotRenderer* renderer)
{
	SpriteRendererComponent::Render(renderer);
}

void SpriteRendererComponent::EditorUIUpdate()
{
	CHECK_RENDERER(GetRenderer());
	UpdateSprite();
	ImGui::SeparatorText("Sprite Size");
	UpdateScale();
	OnConfirmUpdate();
}

void SpriteRendererComponent::RenderDebugGeometries(ImDrawList* imDrawList, FTVector2 screenCenter)
{
	/*FTVector2 ownerPos = GetOwner()->GetTransform()->GetWorldPositionYInverted();
	FTVector2 scale = GetTexture()->GetTexScale() * Camera::GetInstance()->GetNDCRatio();
	FTVector2 min = screenCenter + ownerPos - (scale / 2);
	FTVector2 max = screenCenter + ownerPos + (scale / 2);
	imDrawList->AddRect(
		min.GetImVec2(),
		max.GetImVec2(),
		ImGui::ColorConvertFloat4ToU32(ImVec4(0.f, 0.f, 255.f, 255.f)),
		0.0f,
		0,
		3.0f
	);*/
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
		currentSprite =
			"Current sprite : \n" + ResourceManager::GetInstance()->GetLoadedTexture(GetTexKey())->GetRelativePath();
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
#endif