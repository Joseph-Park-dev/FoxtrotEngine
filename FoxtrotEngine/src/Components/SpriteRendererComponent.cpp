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
	// InitializeMesh(GeometryGenerator::MakeSquare(1.0f));
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
		GeometryGenerator::MakeSquare());
	if (!GetMeshGroup())
	{
		LogString("ERROR: SpriteRendererComponent::InitializeMesh() -> Mesh "
				  "Init failed");
		return false;
	}
	return true;
}

// void SpriteRendererComponent::InitializeMesh()
//{
//	mMesh = new Mesh;
//	MeshData squareMeshData = GeometryGenerator::MakeSquare(mScale);
//	D3D11Utils::CreateVertexBuffer(
//		mRenderer->GetDevice(),
//		squareMeshData.vertices,
//		mMesh->vertexBuffer
//	);
//
//	mMesh->mIndexCount = UINT(squareMeshData.indices.size());
//	mMesh->mVertexCount = squareMeshData.vertices.size();
//
//	D3D11Utils::CreateIndexBuffer(
//		mRenderer->GetDevice(),
//		squareMeshData.indices,
//		mMesh->indexBuffer
//	);
// }

// void SpriteRendererComponent::SetTexture()
//{
//	//rect->x = (int)GetOwner()->GetTransform()->GetWorldPosition().x;
//	//rect->y = (int)GetOwner()->GetTransform()->GetWorldPosition().y;
//	//rect->w = mTextWidth;
//	//rect->h = mTexHeight;
// }

SpriteRendererComponent::SpriteRendererComponent(Actor* owner, int drawOrder,
	int updateOrder)
	: MeshRendererComponent(owner, drawOrder, updateOrder), mChannel(4), mTexScale(FTVector2(1.0f,1.0f)) {}

SpriteRendererComponent::~SpriteRendererComponent() {}

void SpriteRendererComponent::SaveProperties(std::ofstream& ofs)
{
	Component::SaveProperties(ofs);
	FileIOHelper::SaveVector2(ofs, ChunkKeys::TEXTURE_WIDTH, mTexScale);
}

void SpriteRendererComponent::LoadProperties(std::ifstream& ifs)
{
	Component::LoadProperties(ifs);
	// GetMeshArray()[0]->texture->SetTexWidth(FileIOHelper::LoadFloat(ifs));
	// GetMeshArray()[0]->texture->SetTexHeight(FileIOHelper::LoadFloat(ifs));

	// FileIOHelper::LoadFTTexture(ifs, GetMesh()->texture);
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

void SpriteRendererComponent::EditorUIUpdate()
{
	if (GetRenderer())
	{
		UpdateSprite();
		ImGui::SeparatorText("Sprite Size");
		UpdateScale();
	}
}

void SpriteRendererComponent::OnConfirmUpdate()
{
	if (ImGui::Button("Update"))
	{
		this->SetTexture();
	}
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
				SetTexture();
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