#include "Components/SpriteRendererComponent.h"

#include "Actors/Transform.h"
#include "Actors/Actor.h"
#include "Scenes/Scene.h"
#include "Core/FTCore.h"
#include "FileSystem/ChunkLoader.h"
#include "Managers/ResourceManager.h"
#include "ResourceSystem/FTTexture.h"
#include "ResourceSystem/Mesh.h"
#include "ResourceSystem/MeshData.h"
#include "Renderer/GeometryGenerator.h"
#include "Renderer/FoxtrotRenderer.h"
#include "Physics/Bounds.h"
#include "Math/FTMath.h"
#include "Renderer/Camera.h"

SpriteRendererComponent::SpriteRendererComponent(Actor* owner, int drawOrder, int updateOrder)
	: MeshRendererComponent(owner, drawOrder, updateOrder)
	, mTexWidth(0)
	, mTexHeight(0)
	, mChannel(4)
	, mScale(1.f)
{}

SpriteRendererComponent::~SpriteRendererComponent()
{
}

void SpriteRendererComponent::SetTexture(FTTexture* texture)
{
	GetMesh()->texture = texture;
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
		//SetTexture(renderer, fileName);
	}
}

void SpriteRendererComponent::Initialize(FTCore* coreInstance)
{
	MeshRendererComponent::Initialize(coreInstance);
	InitializeMesh(GeometryGenerator::MakeSquare(1.0f));
}


void SpriteRendererComponent::Update(float deltaTime)
{
	UpdateMesh(GetOwner()->GetTransform(), Camera::GetInstance());
}

void SpriteRendererComponent::Render(FoxtrotRenderer* renderer) {
	RenderMesh(renderer);
}

#ifdef _DEBUG
#include "CommandHistory.h"
#include "EditorCamera2D.h"

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
		if (ImGui::Button("Apply")){
			if(GetMesh() && GetMesh()->texture)
				UpdateTexture(GetRenderer(), GetMesh()->texture->GetRelativePath());
		}
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
	}

	if (ImGuiFileDialog::Instance()->Display("SelectSprite")){
		if (ImGuiFileDialog::Instance()->IsOk()){
			std::string spritePath = ImGuiFileDialog::Instance()->GetCurrentPath() + "\\" +
				ImGuiFileDialog::Instance()->GetCurrentFileName();
			//SetTexture(renderer, spritePath);
		}
		ImGuiFileDialog::Instance()->Close();
	}
}

void SpriteRendererComponent::UpdateTexWidth(){
	CommandHistory::GetInstance()->UpdateIntValue("Texture Width", &mTexWidth, INTMOD_SPEED);
}

void SpriteRendererComponent::UpdateTexHeight(){
	CommandHistory::GetInstance()->UpdateIntValue("Texture Height", &mTexHeight, INTMOD_SPEED);
}

void SpriteRendererComponent::UpdateScale(){
	CommandHistory::GetInstance()->UpdateFloatValue("Texture Scale", &mScale, FLOATMOD_SPEED);
}

void SpriteRendererComponent::SaveProperties(std::ofstream& ofs){
	Component::SaveProperties(ofs);
	//FileIOHelper::AddWString(ofs, mTexture->GetKey());
	//FileIOHelper::AddWString(ofs, mTexture->GetRelativePath());
	FileIOHelper::AddFloat(ofs, mTexWidth);
	FileIOHelper::AddInt(ofs, mTexHeight);

	if(GetMesh()->texture)
		FileIOHelper::AddBasicString(ofs, GetMesh()->texture->GetRelativePath());
}

void SpriteRendererComponent::LoadProperties(std::ifstream& ifs){
	Component::LoadProperties(ifs);
	FileIOHelper::LoadInt(ifs, mTexWidth);
	FileIOHelper::LoadInt(ifs, mTexHeight);

	if (GetMesh() && GetMesh()->texture){
		std::string fileName = FileIOHelper::LoadBasicString(ifs);
	}
}
#endif