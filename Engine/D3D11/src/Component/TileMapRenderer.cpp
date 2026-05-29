// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Component/TileMapRenderer.h"

#include <queue>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "Actor/IActor.h"
#include "FTMath.h"
#include "ResourceSystem/Sprite/Tile.h"
#include "ResourceSystem/FTTexture.h"

#include "Actor/Transform.h"
#include "ResourceSystem/Mesh/Mesh.h"
#include "ResourceSystem/Mesh/FTMeshData.h"
#include "ResourceSystem/Mesh/FTMeshGroup.h"
#include "ResourceSystem/Mesh/GeometryGenerator.h"
#include "ResourceSystem/Sprite/FTTileMap.h"
#include "Renderer/Camera.h"
#include "Renderer/IRenderer.h"
#include "Manager/ResourceManager.h"
#include "Renderer/FTRectArea.h"
#include "FileSystem/ChunkLoader.h"
#include "FileSystem/FileIOHelper.h"

#ifdef FOXTROT_EDITOR
	#include "CommandHistory.h"

	#define IMGUI_DEFINE_MATH_OPERATORS
	#include <imgui.h>
	#include <ImGuiFileDialog/ImGuiFileDialog.h>

	#include "FileSystem/FileTypes.h"
#endif

#define DEFAULT_TILE_POS 0

namespace D3D11
{
	using namespace Common;
	using namespace Core;
	Common::FTDS::String& TileMapRenderer::GetTileMapKey()
	{
		return mTileMapKey;
	}

	FTTileMap* TileMapRenderer::GetTileMap() const
	{
		return mTileMap;
	}

	void TileMapRenderer::SetTileMapKey(Common::FTDS::String& key)
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

	void TileMapRenderer::ProcessInput(IInputDevice* inputDevice)
	{
	}

	void TileMapRenderer::Update(float deltaTime)
	{
	}

	void TileMapRenderer::LateUpdate(float deltaTime)
	{
	}

	void TileMapRenderer::Render(Core::IRenderer* renderer, Core::ICamera* camInst)
	{
	}

	void TileMapRenderer::InitializeTileMap()
	{
		if (GetTileMapKey().NotEqual(Common::ChunkKey::NullVal::NULL_OBJECT))
		{
			mTileMap = D3D11::ResourceManager::GetInstance()->GetTileMap(mTileMapKey);
			if (mTileMap)
			{
				mTileMap->Initialize();
				Common::FTDS::DynamicArray<FTMeshData> meshData = GeometryGenerator::MakeTileMapGrid(mTileMap);

				// Need to Implement here

				// MeshRenderer::InitializeMesh(meshData);
			}
		}
	}

	void TileMapRenderer::CloneTo(IActor* actor)
	{
		TileMapRenderer* newComp = DBG_NEW TileMapRenderer(actor, GetUpdateOrder());
		newComp->mTileMapKey	 = this->mTileMapKey;
	}

	TileMapRenderer::TileMapRenderer(IActor* owner, int updateOrder)
		: SpriteRenderer(owner, updateOrder)
		, mTileMap(nullptr)
		, mTileMapKey(Common::ChunkKey::NullVal::NULL_OBJECT)

	{
	}

	TileMapRenderer::~TileMapRenderer()
	{
		// ResourceManager::GetInstance()->RemoveLoadedMeshes(GetMeshKey());
		mTileMap = nullptr;
	}

	void TileMapRenderer::SaveProperties(std::ofstream& ofs)
	{
		D3D11Component::SaveProperties(ofs);
		FileIOHelper::SaveString(ofs, ChunkKey::TileMapRenderer::TILEMAP_KEY, mTileMap->GetFileName());
	}

	void TileMapRenderer::LoadProperties(std::ifstream& ifs)
	{
		Common::FTDS::String texKey;
		FileIOHelper::LoadBasicString(ifs, texKey);
		// SetTexKey(texKey);
		FileIOHelper::LoadBasicString(ifs, mTileMapKey);
		D3D11Component::LoadProperties(ifs);
	}

#ifdef FOXTROT_EDITOR
	void TileMapRenderer::EditorUpdate(float deltaTime)
	{
	}
	void TileMapRenderer::EditorRender(Core::IRenderer* renderer, Core::ICamera* camInst)
	{
	}
	void TileMapRenderer::EditorUIUpdate(Editor::CommandHistory* chInst)
	{
		// UpdateSprite();
		UpdateCSV();

		if (ImGui::Button("Update"))
			this->InitializeTileMap();
	}

	void TileMapRenderer::UpdateCSV()
	{
		Common::FTDS::String currentCSV = "No .csv has been assigned";
		if (mTileMapKey.Equal(Common::ChunkKey::NullVal::NULL_OBJECT))
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
			Common::ResourcePack<D3D11::FTTileMap>* tileMapsMap =
				D3D11::ResourceManager::GetInstance()->GetTileMaps();
			if (ImGui::TreeNode("Selection State: Single Selection"))
			{
				Common::FTDS::String tileMapKey = Common::ChunkKey::NullVal::NULL_OBJECT;
				static int			 selected	= -1;
				int					 i			= 0;
				for (auto iter = tileMapsMap->GetResMap()->Begin(); iter != tileMapsMap->GetResMap()->End();
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
					SetTileMapKey(tileMapKey);
					// SetTexture();
				}
			}
			if (ImGui::Button("Close"))
				ImGui::CloseCurrentPopup();
			ImGui::EndPopup();
		}
	}
	void TileMapRenderer::UpdateCSV(Common::FTDS::String& key)
	{
		Common::FTDS::String currentCSV = {};
		if (key.NotEqual(Common::ChunkKey::NullVal::NULL_OBJECT))
			currentCSV =
				Common::FTDS::String("Current sprite : \n") +
				D3D11::ResourceManager::GetInstance()->GetTileMap(key)->GetRelativePath()->C_Str();
		else
			currentCSV = "No .csv has been assigned";
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
			Common::ResourcePack<D3D11::FTTileMap>* tileMapsMap =
				D3D11::ResourceManager::GetInstance()->GetTileMaps();
			if (ImGui::TreeNode("Selection State: Single Selection"))
			{
				Common::FTDS::String tileMapKey = Common::ChunkKey::NullVal::NULL_OBJECT;
				static int			 selected	= -1;
				int					 i			= 0;
				for (auto iter = tileMapsMap->GetResMap()->Begin(); iter != tileMapsMap->GetResMap()->End();
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