#include "FoxtrotEngine/Components/SpriteRendererComponent.h"

#include "FoxtrotEngine/Actors/Transform.h"
#include "FoxtrotEngine/Actors/Actor.h"
#include "FoxtrotEngine/Scenes/Scene.h"
#include "FoxtrotEngine/Core/FTCore.h"
#include "FoxtrotEngine/FileSystem/ChunkLoader.h"
#include "FoxtrotEngine/Managers/ResourceManager.h"
#include "FoxtrotEngine/ResourceSystem/FTTexture.h"
#include "FoxtrotEngine/ResourceSystem/Mesh.h"
#include "FoxtrotEngine/ResourceSystem/MeshData.h"
#include "FoxtrotEngine/Renderer/GeometryGenerator.h"
#include "FoxtrotEngine/Renderer/FoxtrotRenderer.h"
#include "FoxtrotEngine/Physics/Bounds.h"
#include "FoxtrotEngine/Math/FTMath.h"
#include "FoxtrotEngine/Renderer/Camera.h"

SpriteRendererComponent::SpriteRendererComponent(Actor* owner, int drawOrder, int updateOrder)
	: MeshRendererComponent(owner, drawOrder, updateOrder)
	, mChannel(4)
	, mScale(1.f)
{}

SpriteRendererComponent::~SpriteRendererComponent()
{
}

void SpriteRendererComponent::UpdateTexture(FoxtrotRenderer* renderer, std::string fileName)
{
	std::vector<Mesh*>& meshes = renderer->GetMeshes();
	std::vector<Mesh*>::iterator iter = std::find(meshes.begin(), meshes.end(), GetMesh());
	if (iter != meshes.end())
	{
		delete (*iter);
		meshes.erase(iter);

		//FTTexture* texture = (*iter)->texture;
		//ResourceManager::GetInstance()->UpdateTexture(renderer, texture, channels);
		//mMesh->texture = ResourceManager::GetInstance()->GetLoadedTexture(fileName);
	}
}

void SpriteRendererComponent::Initialize(FTCore* coreInstance)
{
	MeshRendererComponent::Initialize(coreInstance);
	InitializeMesh(GeometryGenerator::MakeSquare(1.0f));
}


void SpriteRendererComponent::Update(float deltaTime)
{
	MeshRendererComponent::UpdateMesh(GetOwner()->GetTransform(), Camera::GetInstance());
}

void SpriteRendererComponent::Render(FoxtrotRenderer* renderer) {
	MeshRendererComponent::Render(renderer);
}

#ifdef _DEBUG
#include "FoxtrotEditor/CommandHistory.h"
#include "FoxtrotEditor/EditorCamera2D.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include "imgui/FileDialog/ImGuiFileDialog.h"
#include "imgui/FileDialog/ImGuiFileDialogConfig.h"

//void SpriteRendererComponent::SetTexture()
//{
//	//rect->x = (int)GetOwner()->GetTransform()->GetWorldPosition().x;
//	//rect->y = (int)GetOwner()->GetTransform()->GetWorldPosition().y;
//	//rect->w = mTextWidth;
//	//rect->h = mTexHeight;
//}

void SpriteRendererComponent::EditorUpdate(float deltaTime)
{
	Update(deltaTime);
}

void SpriteRendererComponent::EditorUIUpdate()
{
	if (GetRenderer())
	{
		UpdateSprite(GetRenderer());
		ImGui::SeparatorText("Sprite Size");
		UpdateScale();
	}
}

void SpriteRendererComponent::UpdateSprite(FoxtrotRenderer* renderer)
{
	std::string currentSprite = "No sprite has been assigned";
	if (GetMesh()->texture)
		currentSprite = "Current sprite : \n" + GetMesh()->texture->GetRelativePath();
	ImGui::Text(currentSprite.c_str());

	if (ImGui::Button("Select Sprite")){
		IGFD::FileDialogConfig config;
		config.path = ".";
		config.countSelectionMax = 1;
		ImGuiFileDialog::Instance()->OpenDialog("SelectSprite", "Select Sprite", SPRITE_FORMAT_SUPPORTED, config);
		ImGui::OpenPopup("Select Sprite");
		
	}

	if (ImGui::BeginPopupModal("Select Sprite", NULL, ImGuiWindowFlags_MenuBar)) {
		std::unordered_map<std::string, FTTexture*>& texturesMap = ResourceManager::GetInstance()->GetTexturesMap();
		if (ImGui::TreeNode("Selection State: Single Selection"))
		{
			std::string spriteName = {};
			static int selected = -1;
		    int i = 0;
			for (auto iter = texturesMap.begin(); iter != texturesMap.end(); ++iter, ++i)
			{
				if (ImGui::Selectable((*iter).first.c_str(), selected == i))
				{
					spriteName = (*iter).first;
					selected = i;
				}
			}
			ImGui::TreePop();
			if (selected != -1) {
				SetTexture(texturesMap[spriteName]);
			}
		}
		if (ImGui::Button("Close"))
			ImGui::CloseCurrentPopup();
		ImGui::EndPopup();
	}

	//if (ImGuiFileDialog::Instance()->Display("SelectSprite")){
	//	if (ImGuiFileDialog::Instance()->IsOk()){
	//		std::string spritePath = ImGuiFileDialog::Instance()->GetCurrentPath() + "\\" +
	//			ImGuiFileDialog::Instance()->GetCurrentFileName();
	//		SetTexture(renderer, spritePath);
	//	}
	//	ImGuiFileDialog::Instance()->Close();
	//}
}

void SpriteRendererComponent::UpdateTexWidth(){
	CommandHistory::GetInstance()->UpdateIntValue("Texture Width", &GetMesh()->texture->GetTexWidthRef(), INTMOD_SPEED);
}

void SpriteRendererComponent::UpdateTexHeight(){
	CommandHistory::GetInstance()->UpdateIntValue("Texture Height", &GetMesh()->texture->GetTexHeightRef(), INTMOD_SPEED);
}

void SpriteRendererComponent::UpdateScale(){
	CommandHistory::GetInstance()->UpdateFloatValue("Texture Scale", &mScale, FLOATMOD_SPEED);
}

void SpriteRendererComponent::SaveProperties(std::ofstream& ofs){
	Component::SaveProperties(ofs);
	//FileIOHelper::AddWString(ofs, mTexture->GetKey());
	//FileIOHelper::AddWString(ofs, mTexture->GetRelativePath());
	FileIOHelper::AddFloat(ofs, GetMesh()->texture->GetTexWidth());
	FileIOHelper::AddFloat(ofs, GetMesh()->texture->GetTexHeight());

	//FileIOHelper::AddFTTexture(ofs, GetMesh()->texture);
}

void SpriteRendererComponent::LoadProperties(std::ifstream& ifs){
	Component::LoadProperties(ifs);
	GetMesh()->texture->SetTexWidth(FileIOHelper::LoadFloat(ifs));
	GetMesh()->texture->SetTexHeight(FileIOHelper::LoadFloat(ifs));

	//FileIOHelper::LoadFTTexture(ifs, GetMesh()->texture);
}
#endif