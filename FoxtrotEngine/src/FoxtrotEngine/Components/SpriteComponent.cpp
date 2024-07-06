#include "FoxtrotEngine/Components/SpriteComponent.h"

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

SpriteComponent::SpriteComponent(Actor* owner, int drawOrder, int updateOrder)
	: MeshRendererComponent(owner, drawOrder, updateOrder)
	, mTexWidth(0)
	, mTexHeight(0)
	, mChannel(4)
	, mScale(1.f)
{}

SpriteComponent::~SpriteComponent()
{
}

void SpriteComponent::SetTexture(FoxtrotRenderer* renderer, std::string fileName)
{
	std::vector<Mesh*>& meshes = renderer->GetMeshes();
	std::vector<Mesh*>::iterator iter = std::find(meshes.begin(), meshes.end(), GetMesh());
	if (iter != meshes.end())
		GetMesh()->texture = ResourceManager::GetInstance()->GetLoadedTexture(fileName);
}

void SpriteComponent::UpdateTexture(FoxtrotRenderer* renderer, std::string fileName)
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
		SetTexture(renderer, fileName);
	}
}

void SpriteComponent::Initialize(FTCore* coreInstance)
{
	MeshRendererComponent::Initialize(coreInstance);
	MeshRendererComponent::InitializeMesh(GeometryGenerator::MakeSquare(1.0f));
}

void SpriteComponent::Update(float deltaTime)
{
	MeshRendererComponent::UpdateMesh(GetOwner()->GetTransform(), Camera::GetInstance());
}

#ifdef _DEBUG
#include "FoxtrotEditor/CommandHistory.h"
#include "FoxtrotEditor/EditorCamera2D.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include "imgui/FileDialog/ImGuiFileDialog.h"
#include "imgui/FileDialog/ImGuiFileDialogConfig.h"

//void SpriteComponent::SetTexture()
//{
//	//rect->x = (int)GetOwner()->GetTransform()->GetWorldPosition().x;
//	//rect->y = (int)GetOwner()->GetTransform()->GetWorldPosition().y;
//	//rect->w = mTextWidth;
//	//rect->h = mTexHeight;
//}

void SpriteComponent::EditorUpdate(float deltaTime)
{
	Update(deltaTime);
}

void SpriteComponent::EditorUIUpdate()
{
	if (GetRenderer())
	{
		UpdateSprite(GetRenderer());
		ImGui::SeparatorText("Sprite Size");
		UpdateScale();
		if (ImGui::Button("Apply")){
			if(GetMesh() && GetMesh()->texture)
				UpdateTexture(GetRenderer(), GetMesh()->texture->GetRelativePath());
		}
	}
}

void SpriteComponent::UpdateSprite(FoxtrotRenderer* renderer)
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
	}

	if (ImGuiFileDialog::Instance()->Display("SelectSprite")){
		if (ImGuiFileDialog::Instance()->IsOk()){
			std::string spritePath = ImGuiFileDialog::Instance()->GetCurrentPath() + "\\" +
				ImGuiFileDialog::Instance()->GetCurrentFileName();
			SetTexture(renderer, spritePath);
		}
		ImGuiFileDialog::Instance()->Close();
	}
}

void SpriteComponent::UpdateTexWidth(){
	CommandHistory::GetInstance()->UpdateIntValue("Texture Width", &mTexWidth, INTMOD_SPEED);
}

void SpriteComponent::UpdateTexHeight(){
	CommandHistory::GetInstance()->UpdateIntValue("Texture Height", &mTexHeight, INTMOD_SPEED);
}

void SpriteComponent::UpdateScale(){
	CommandHistory::GetInstance()->UpdateFloatValue("Texture Scale", &mScale, FLOATMOD_SPEED);
}

void SpriteComponent::SaveProperties(std::ofstream& ofs){
	Component::SaveProperties(ofs);
	//FileIOHelper::AddWString(ofs, mTexture->GetKey());
	//FileIOHelper::AddWString(ofs, mTexture->GetRelativePath());
	FileIOHelper::AddFloat(ofs, mTexWidth);
	FileIOHelper::AddInt(ofs, mTexHeight);

	if(GetMesh()->texture)
		FileIOHelper::AddBasicString(ofs, GetMesh()->texture->GetRelativePath());
}

void SpriteComponent::LoadProperties(std::ifstream& ifs){
	Component::LoadProperties(ifs);
	FileIOHelper::LoadInt(ifs, mTexWidth);
	FileIOHelper::LoadInt(ifs, mTexHeight);

	if (GetMesh() && GetMesh()->texture){
		std::string fileName = FileIOHelper::LoadBasicString(ifs);
		SetTexture(GetRenderer(), fileName);
	}
}
#endif