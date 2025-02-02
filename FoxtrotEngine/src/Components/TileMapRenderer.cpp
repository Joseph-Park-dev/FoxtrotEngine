// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Components/TileMapRenderer.h"

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

UINT TileMapRenderer::GetTileMapKey() const
{
	return mTileMapKey;
}

FTTileMap* TileMapRenderer::GetTileMap() const
{
	return mTileMap;
}

void TileMapRenderer::SetTileMapKey(UINT key)
{
	mTileMapKey = key;
}

void TileMapRenderer::SetTileMap(FTTileMap* tileMap)
{
	mTileMap = tileMap;
}

void TileMapRenderer::Initialize(FTCore* coreInstance)
{
    MeshRenderer::Initialize(coreInstance);
	if (GetTexKey() != ChunkKeys::VALUE_NOT_ASSIGNED && mTileMapKey != ChunkKeys::VALUE_NOT_ASSIGNED)
		this->InitializeTileMap();
}

void TileMapRenderer::InitializeTileMap() {
	if (GetTileMapKey() != ChunkKeys::VALUE_NOT_ASSIGNED)
	{
		mTileMap = ResourceManager::GetInstance()->GetLoadedTileMap(mTileMapKey);
		if (mTileMap)
		{
			if (GetTexKey() != ChunkKeys::VALUE_NOT_ASSIGNED)
				SetTexture();
			mTileMap->ReadCSV();
			SetMeshKey(ChunkKeys::PRIMITIVE_SQUARE_BLUE);
			std::vector<MeshData> meshData = GeometryGenerator::MakeTileMapGrid(mTileMap);
			MeshRenderer::InitializeMesh(meshData);
		}
	}
}

void TileMapRenderer::CloneTo(Actor* actor)
{
	TileMapRenderer* newComp = DBG_NEW TileMapRenderer(actor, GetUpdateOrder());
	newComp->mTileMapKey = this->mTileMapKey;
}

TileMapRenderer::TileMapRenderer(Actor* owner, int updateOrder)
    : SpriteRenderer(owner, updateOrder)
    , mTileMap(nullptr)
    , mTileMapKey(ChunkKeys::VALUE_NOT_ASSIGNED)

{}

TileMapRenderer::~TileMapRenderer()
{
    //ResourceManager::GetInstance()->RemoveLoadedMeshes(GetMeshKey());
	mTileMap = nullptr;
}

void TileMapRenderer::SaveProperties(std::ofstream& ofs)
{
	Component::SaveProperties(ofs);
    FileIOHelper::SaveUnsignedInt(ofs, ChunkKeys::TILEMAP_KEY, mTileMapKey);
	FileIOHelper::SaveUnsignedInt(ofs, ChunkKeys::MESH_KEY, GetTexKey());
}

void TileMapRenderer::LoadProperties(std::ifstream& ifs)
{
	UINT texKey = 0;
	FileIOHelper::LoadUnsignedInt(ifs, texKey);
	SetTexKey(texKey);
    FileIOHelper::LoadUnsignedInt(ifs, mTileMapKey);
	Component::LoadProperties(ifs);
}

#ifdef FOXTROT_EDITOR
void TileMapRenderer::EditorUIUpdate()
{
    CHECK_RENDERER(GetRenderer());
    UpdateSprite();
    UpdateCSV();
    OnConfirmUpdate();
}

void TileMapRenderer::OnConfirmUpdate()
{
    if (ImGui::Button("Update"))
        this->InitializeTileMap();
}

void TileMapRenderer::UpdateCSV() {
	std::string currentCSV = "No .csv has been assigned";
	if (mTileMapKey != ChunkKeys::VALUE_NOT_ASSIGNED)
		currentCSV =
		"Current sprite : \n" + ResourceManager::GetInstance()->GetLoadedTileMap(mTileMapKey)->GetRelativePath();
	ImGui::Text(currentCSV.c_str());

	if (ImGui::Button("Select .CSV"))
	{
		IGFD::FileDialogConfig config;
		config.path = ".";
		config.countSelectionMax = 1;
		ImGuiFileDialog::Instance()->OpenDialog(
			"SelectCSV", "Select .CSV", ChunkKeys::TEXTURE_FORMAT_SUPPORTED, config);
		ImGui::OpenPopup("Select .CSV");
	}

	if (ImGui::BeginPopupModal("Select .CSV", NULL,
		ImGuiWindowFlags_MenuBar))
	{
		std::unordered_map<UINT, FTTileMap*>& tileMapsMap =
			ResourceManager::GetInstance()->GetTileMapsMap();
		if (ImGui::TreeNode("Selection State: Single Selection"))
		{
			UINT	   tileMapKey = ChunkKeys::VALUE_NOT_ASSIGNED;
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
void TileMapRenderer::UpdateCSV(UINT& key)
{
	std::string currentCSV = {};
	if (key != ChunkKeys::VALUE_NOT_ASSIGNED)
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
			"SelectCSV", "Select .CSV", ChunkKeys::TEXTURE_FORMAT_SUPPORTED, config);
		ImGui::OpenPopup("Select .CSV");
	}

	if (ImGui::BeginPopupModal("Select .CSV", NULL,
		ImGuiWindowFlags_MenuBar))
	{
		std::unordered_map<UINT, FTTileMap*>& tileMapsMap =
			ResourceManager::GetInstance()->GetTileMapsMap();
		if (ImGui::TreeNode("Selection State: Single Selection"))
		{
			UINT	   tileMapKey = ChunkKeys::VALUE_NOT_ASSIGNED;
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