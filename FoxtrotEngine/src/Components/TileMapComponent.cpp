#include "Components/TileMapComponent.h"

#include <queue>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "Actors/Actor.h"
#include "Math/FTMath.h"
#include "ResourceSystem/Tile.h"
#include "ResourceSystem/FTTexture.h"

#include "Actors/Transform.h"
#include "Core/FTCore.h"
#include "ResourceSystem/Mesh.h"
#include "ResourceSystem/FTBasicMeshGroup.h"
#include "ResourceSystem/GeometryGenerator.h"
#include "Renderer/Camera.h"
#include "Renderer/FoxtrotRenderer.h"
#include "Managers/ResourceManager.h"
#include "Renderer/FTRect.h"
#include "FileSystem/ChunkLoader.h"
#include "FileSystem/ChunkFileKeys.h"
#include "FileSystem/FileIOHelper.h"

#ifdef FOXTROT_EDITOR
#include "CommandHistory.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include "imgui/FileDialog/ImGuiFileDialog.h"
#include "imgui/FileDialog/ImGuiFileDialogConfig.h"
#endif 

#define DEFAULT_TILE_POS 0

UINT TileMapComponent::GetTileMapKey() const
{
	return mTileMapKey;
}

FTTileMap* TileMapComponent::GetTileMap() const
{
	return mTileMap;
}

void TileMapComponent::Initialize(FTCore* coreInstance)
{
    MeshRendererComponent::Initialize(coreInstance);
}

void TileMapComponent::InitializeTileMap() {
    assert(mTileMapKey != VALUE_NOT_ASSIGNED);
    assert(GetTexKey() != VALUE_NOT_ASSIGNED);
    //assert(GetMeshKey() != VALUE_NOT_ASSIGNED);
    mTileMap = ResourceManager::GetInstance()->GetLoadedTileMap(mTileMapKey);
    if (mTileMap)
    {
		SetTexture();
        mTileMap->ReadCSV();
		SetMeshKey(ChunkKeys::PRIMITIVE_SQUARE_BLUE);
        SpriteRendererComponent::InitializeMesh();
        //GetOwner()->GetTransform()->SetScale(FTVector3(0.03f, 0.03f, 1.0f));
    }
}

void TileMapComponent::Update(float deltaTime)
{
    if (GetMeshGroup())
        MeshRendererComponent::Update(deltaTime);
}

void TileMapComponent::Render(FoxtrotRenderer* renderer)
{
    MeshRendererComponent::Render(renderer);
}

TileMapComponent::TileMapComponent(Actor* owner, int drawOrder, int UpdateOrder)
    : SpriteRendererComponent(owner, drawOrder)
    , mTileMap(nullptr)
    , mTileMapKey(VALUE_NOT_ASSIGNED)

{}

TileMapComponent::~TileMapComponent()
{
    //ResourceManager::GetInstance()->RemoveLoadedMeshes(GetMeshKey());
	mTileMap = nullptr;
}

void TileMapComponent::SaveProperties(std::ofstream& ofs)
{
    Component::SaveProperties(ofs);
    FileIOHelper::SaveUnsignedInt(ofs, ChunkKeys::TILEMAP_KEY, mTileMapKey);
	FileIOHelper::SaveUnsignedInt(ofs, ChunkKeys::MESH_KEY, GetTexKey());
}

void TileMapComponent::LoadProperties(std::ifstream& ifs)
{
	UINT texKey = 0;
	FileIOHelper::LoadUnsignedInt(ifs, texKey);
	SetTexKey(texKey);
    FileIOHelper::LoadUnsignedInt(ifs, mTileMapKey);
    Component::LoadProperties(ifs);

	if(GetTexKey() != VALUE_NOT_ASSIGNED && mTileMapKey != VALUE_NOT_ASSIGNED)
		this->InitializeTileMap();
}

#ifdef FOXTROT_EDITOR
void TileMapComponent::EditorUIUpdate()
{
    CHECK_RENDERER(GetRenderer());
    UpdateSprite();
    UpdateCSV();
    OnConfirmUpdate();
}

void TileMapComponent::CloneTo(Actor* actor)
{
	TileMapComponent* newComp = DBG_NEW TileMapComponent(actor, GetDrawOrder(), GetUpdateOrder());
	newComp->mTileMapKey = this->mTileMapKey;
}

void TileMapComponent::OnConfirmUpdate()
{
    if (ImGui::Button("Update"))
        this->InitializeTileMap();
}

void TileMapComponent::UpdateCSV() {
	std::string currentCSV = "No .csv has been assigned";
	if (mTileMapKey != VALUE_NOT_ASSIGNED)
		currentCSV =
		"Current sprite : \n" + ResourceManager::GetInstance()->GetLoadedTileMap(mTileMapKey)->GetRelativePath();
	ImGui::Text(currentCSV.c_str());

	if (ImGui::Button("Select .CSV"))
	{
		IGFD::FileDialogConfig config;
		config.path = ".";
		config.countSelectionMax = 1;
		ImGuiFileDialog::Instance()->OpenDialog(
			"SelectCSV", "Select .CSV", TEXTURE_FORMAT_SUPPORTED, config);
		ImGui::OpenPopup("Select .CSV");
	}

	if (ImGui::BeginPopupModal("Select .CSV", NULL,
		ImGuiWindowFlags_MenuBar))
	{
		std::unordered_map<UINT, FTTileMap*>& tileMapsMap =
			ResourceManager::GetInstance()->GetTileMapsMap();
		if (ImGui::TreeNode("Selection State: Single Selection"))
		{
			UINT	   tileMapKey = VALUE_NOT_ASSIGNED;
			static int selected = -1;
			int		   i = 0;
			for (auto iter = tileMapsMap.begin(); iter != tileMapsMap.end();
				++iter, ++i)
			{
				if (ImGui::Selectable((*iter).second->GetFileName().c_str(),
					selected == i))
				{
					tileMapKey = (*iter).first;
					selected = i;
				}
			}
			ImGui::TreePop();
			if (selected != -1)
			{
				mTileMapKey = tileMapKey;
				//SetTexture();
			}
		}
		if (ImGui::Button("Close"))
			ImGui::CloseCurrentPopup();
		ImGui::EndPopup();
	}
}
void TileMapComponent::UpdateCSV(UINT& key)
{
	std::string currentCSV = {};
	if (key != VALUE_NOT_ASSIGNED)
		currentCSV =
		"Current sprite : \n" + ResourceManager::GetInstance()->GetLoadedTileMap(key)->GetRelativePath();
	else
		currentCSV = "No .csv has been assigned";
	ImGui::Text(currentCSV.c_str());

	if (ImGui::Button("Select .CSV"))
	{
		IGFD::FileDialogConfig config;
		config.path = ".";
		config.countSelectionMax = 1;
		ImGuiFileDialog::Instance()->OpenDialog(
			"SelectCSV", "Select .CSV", TEXTURE_FORMAT_SUPPORTED, config);
		ImGui::OpenPopup("Select .CSV");
	}

	if (ImGui::BeginPopupModal("Select .CSV", NULL,
		ImGuiWindowFlags_MenuBar))
	{
		std::unordered_map<UINT, FTTileMap*>& tileMapsMap =
			ResourceManager::GetInstance()->GetTileMapsMap();
		if (ImGui::TreeNode("Selection State: Single Selection"))
		{
			UINT	   tileMapKey = VALUE_NOT_ASSIGNED;
			static int selected = -1;
			int		   i = 0;
			for (auto iter = tileMapsMap.begin(); iter != tileMapsMap.end();
				++iter, ++i)
			{
				if (ImGui::Selectable((*iter).second->GetFileName().c_str(),
					selected == i))
				{
					tileMapKey = (*iter).first;
					selected = i;
				}
			}
			ImGui::TreePop();
			if (selected != -1)
			{
				key = tileMapKey;
				//SetTexture();
			}
		}
		if (ImGui::Button("Close"))
			ImGui::CloseCurrentPopup();
		ImGui::EndPopup();
	}
}
#endif