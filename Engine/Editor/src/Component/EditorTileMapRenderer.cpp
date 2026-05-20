// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Component/EditorTileMapRenderer.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui/imgui.h>
#include <ImGuiFileDialog/ImGuiFileDialog.h>

#include "CommandHistory.h"
#include "FileSystem/FileTypes.h"
#include "FileSystem/NullKeys.h"
#include "ResourceSystem/Sprite/FTTileMap.h"

#include <../D3D11/include/Manager/ResourceManager.h>

#define DEFAULT_TILE_POS 0

namespace Editor
{
	void EditorTileMapRenderer::EditorUIUpdate()
	{
		// UpdateSprite();
		UpdateCSV();

		if (ImGui::Button("Update"))
			this->InitializeTileMap();
	}

	void EditorTileMapRenderer::UpdateCSV()
	{
		Common::FTDS::String currentCSV = "No .csv has been assigned";
		if (TileMapKey().Equal(Common::ChunkKey::NullVal::NULL_OBJECT))
		{
			currentCSV.Assign("Current sprite : \n");
			currentCSV.Append(TileMapKey());
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
					mTileMapKey = tileMapKey;
					// SetTexture();
				}
			}
			if (ImGui::Button("Close"))
				ImGui::CloseCurrentPopup();
			ImGui::EndPopup();
		}
	}
	void EditorTileMapRenderer::UpdateCSV(Common::FTDS::String& key)
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
} // namespace D3D11