// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "TileMapRenderer.h"

#include <queue>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "Actor/Actor.h"
#include "Math/FTMath.h"
#include "ResourceSystem/Sprite/Tile.h"
#include "ResourceSystem/FTTexture.h"

#include "Actor/Transform.h"
#include "FTCore.h"
#include "ResourceSystem/Mesh/Mesh.h"
#include "ResourceSystem/Mesh/FTMeshData.h"
#include "ResourceSystem/Mesh/FTMeshGroup.h"
#include "ResourceSystem/Mesh/GeometryGenerator.h"
#include "ResourceSystem/Sprite/FTTileMap.h"
#include "Renderer/Camera.h"
#include "Renderer/FoxtrotRenderer.h"
#include "Manager/ResourceManager.h"
#include "Renderer/FTRectArea.h"
#include "FileSystem/ChunkLoader.h"
#include "FileSystem/FileIOHelper.h"

#ifdef FOXTROT_EDITOR
	#include "CommandHistory.h"

	#define IMGUI_DEFINE_MATH_OPERATORS
	#include <imgui/imgui.h>
	#include <imgui/ImGuiFileDialog/ImGuiFileDialog.h>

	#include "FileSystem/FileTypes.h"
#endif

#define DEFAULT_TILE_POS 0

namespace D3D11
{
	using namespace Core;
	FTDS::String& TileMapRenderer::GetTileMapKey()
	{
		return mTileMapKey;
	}

	FTTileMap* TileMapRenderer::GetTileMap() const
	{
		return mTileMap;
	}

	void TileMapRenderer::SetTileMapKey(FTDS::String& key)
	{
		mTileMapKey = key;
	}

	void TileMapRenderer::SetTileMap(FTTileMap* tileMap)
	{
		mTileMap = tileMap;
	}

	void TileMapRenderer::Initialize()
	{
		SpriteRenderer::Initialize();
		this->InitializeTileMap();
	}

	void TileMapRenderer::ProcessInput(FTInputDevice* inputDevice)
	{
	}

	void TileMapRenderer::Update(float deltaTime)
	{
	}

	void TileMapRenderer::LateUpdate(float deltaTime)
	{
	}

	void TileMapRenderer::Render(D3D11::D3D11Renderer* renderer)
	{
	}

	void TileMapRenderer::InitializeTileMap()
	{
		if (GetTileMapKey().NotEqual(Core::ChunkKey::NullVal::NULL_OBJECT))
		{
			mTileMap = D3D11::ResourceManager::GetInstance()->GetResource<FTTileMap>(mTileMapKey);
			if (mTileMap)
			{
				mTileMap->Initialize();
				FTDS::DynamicArray<FTMeshData> meshData = GeometryGenerator::MakeTileMapGrid(mTileMap);

				// Need to Implement here

				// MeshRenderer::InitializeMesh(meshData);
			}
		}
	}

	void TileMapRenderer::CloneTo(Actor* actor)
	{
		TileMapRenderer* newComp = DBG_NEW TileMapRenderer(GetPlugin(), actor, GetUpdateOrder());
		newComp->mTileMapKey	 = this->mTileMapKey;
	}

	TileMapRenderer::TileMapRenderer(Plugin* plugin, Actor* owner, int updateOrder)
		: SpriteRenderer(plugin, owner, updateOrder)
		, mTileMap(nullptr)
		, mTileMapKey(Core::ChunkKey::NullVal::NULL_OBJECT)

	{
	}

	TileMapRenderer::~TileMapRenderer()
	{
		// ResourceManager::GetInstance()->RemoveLoadedMeshes(GetMeshKey());
		mTileMap = nullptr;
	}

	void TileMapRenderer::SaveProperties(std::ofstream& ofs)
	{
		Component::SaveProperties(ofs);
		FileIOHelper::SaveString(ofs, ChunkKey::TILEMAP_KEY, D3D11::RES_NAME(FTTileMap, mTileMap));
	}

	void TileMapRenderer::LoadProperties(std::ifstream& ifs)
	{
		FTDS::String texKey;
		FileIOHelper::LoadBasicString(ifs, texKey);
		// SetTexKey(texKey);
		FileIOHelper::LoadBasicString(ifs, mTileMapKey);
		Component::LoadProperties(ifs);
	}

#ifdef FOXTROT_EDITOR
	void TileMapRenderer::EditorUIUpdate()
	{
		// UpdateSprite();
		UpdateCSV();
		OnConfirmUpdate();
	}

	void TileMapRenderer::OnConfirmUpdate()
	{
		if (ImGui::Button("Update"))
			this->InitializeTileMap();
	}

	void TileMapRenderer::UpdateCSV()
	{
		FTDS::String currentCSV = "No .csv has been assigned";
		if (mTileMapKey.Equal(Core::ChunkKey::NullVal::NULL_OBJECT))
		{
			currentCSV.Assign("Current sprite : \n");
			currentCSV.Append(mTileMapKey);
		}

		ImGui::Text(currentCSV.C_Str());

		if (ImGui::Button("Select .CSV"))
		{
			IGFD::FileDialogConfig config;
			config.path				 = ".";
			config.countSelectionMax = 1;
			ImGuiFileDialog::Instance()->OpenDialog(
				"SelectCSV", "Select .CSV", Core::FileTypes::TEXTURE, config);
			ImGui::OpenPopup("Select .CSV");
		}

		if (ImGui::BeginPopupModal("Select .CSV", NULL, ImGuiWindowFlags_MenuBar))
		{
			FTDS::HashMap<FTResource*>& tileMapsMap =
				D3D11::ResourceManager::GetInstance()->GetResMap<FTTileMap>();
			if (ImGui::TreeNode("Selection State: Single Selection"))
			{
				FTDS::String tileMapKey = Core::ChunkKey::NullVal::NULL_OBJECT;
				static int	 selected	= -1;
				int			 i			= 0;
				for (auto iter = tileMapsMap.Begin(); iter != tileMapsMap.End();
					 ++iter, ++i)
				{
					if (ImGui::Selectable((*iter)->Value()->GetFileName()->C_Str(), selected == i))
					{
						tileMapKey = *(*iter)->Value()->GetFileName();
						selected   = i;
					}
				}
				ImGui::TreePop();
				if (selected != -1)
				{
					mTileMapKey = tileMapKey;
					// SetTexture();
				}
			}
			if (ImGui::Button("Close"))
				ImGui::CloseCurrentPopup();
			ImGui::EndPopup();
		}
	}
	void TileMapRenderer::UpdateCSV(FTDS::String& key)
	{
		FTDS::String currentCSV = {};
		if (key.NotEqual(Core::ChunkKey::NullVal::NULL_OBJECT))
			currentCSV =
				FTDS::String("Current sprite : \n") +
				Core::ResourceManager::GetInstance()->GetResource<FTTileMap>(key)->GetRelativePath()->C_Str();
		else
			currentCSV = "No .csv has been assigned";
		ImGui::Text(currentCSV.C_Str());

		if (ImGui::Button("Select .CSV"))
		{
			IGFD::FileDialogConfig config;
			config.path				 = ".";
			config.countSelectionMax = 1;
			ImGuiFileDialog::Instance()->OpenDialog(
				"SelectCSV", "Select .CSV", FileTypes::TEXTURE, config);
			ImGui::OpenPopup("Select .CSV");
		}

		if (ImGui::BeginPopupModal("Select .CSV", NULL, ImGuiWindowFlags_MenuBar))
		{
			FTDS::HashMap<FTResource*>& tileMapsMap =
				Core::ResourceManager::GetInstance()->GetResMap<FTTileMap>();
			if (ImGui::TreeNode("Selection State: Single Selection"))
			{
				FTDS::String tileMapKey = Core::ChunkKey::NullVal::NULL_OBJECT;
				static int	 selected	= -1;
				int			 i			= 0;
				for (auto iter = tileMapsMap.Begin(); iter != tileMapsMap.End();
					 ++iter, ++i)
				{
					if ((*iter)->Value())
					{
						if (ImGui::Selectable((*iter)->Value()->GetFileName()->C_Str(), selected == i))
						{
							tileMapKey = *(*iter)->Value()->GetFileName();
							selected   = i;
						}
					}
				}
				ImGui::TreePop();
				if (selected != -1)
				{
					key = tileMapKey;
					// SetTexture();
				}
			}
			if (ImGui::Button("Close"))
				ImGui::CloseCurrentPopup();
			ImGui::EndPopup();
		}
	}
#endif
} // namespace D3D11