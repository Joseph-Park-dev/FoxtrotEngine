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
	/// @brief Returns the tile map key used by this tile map renderer.
	/// @return Borrowed access to the tile map key.
	/// @note Changes through the returned reference affect this object's stored state.
	Common::FTDS::String& TileMapRenderer::GetTileMapKey()
	{
		return mTileMapKey;
	}

	/// @brief Returns the tile map used by this tile map renderer.
	/// @return Borrowed access to the tile map.
	FTTileMap* TileMapRenderer::GetTileMap() const
	{
		return mTileMap;
	}

	/// @brief Updates the tile map key used by subsequent operations.
	/// @param key Replacement tile map key.
	void TileMapRenderer::SetTileMapKey(Common::FTDS::String& key)
	{
		mTileMapKey = key;
	}

	/// @brief Updates the tile map used by subsequent operations.
	/// @param tileMap Replacement tile map.
	void TileMapRenderer::SetTileMap(FTTileMap* tileMap)
	{
		mTileMap = tileMap;
	}

	/// @brief Initializes the services and state required before this object's runtime lifecycle begins.
	void TileMapRenderer::Initialize()
	{
		SpriteRenderer::Initialize();
		this->InitializeTileMap();
	}

	/// @brief Provides an empty lifecycle or extension hook for this implementation.
	/// @param inputDevice Device exposing the current frame's input state.
	void TileMapRenderer::ProcessInput(IInputDevice* inputDevice)
	{
	}

	/// @brief Provides an empty lifecycle or extension hook for this implementation.
	/// @param deltaTime Elapsed frame time in seconds.
	void TileMapRenderer::Update(float deltaTime)
	{
	}

	/// @brief Provides an empty lifecycle or extension hook for this implementation.
	/// @param deltaTime Elapsed frame time in seconds.
	void TileMapRenderer::LateUpdate(float deltaTime)
	{
	}

	/// @brief Provides an empty lifecycle or extension hook for this implementation.
	/// @param renderer Renderer providing the graphics device and current render state.
	/// @param camInst Camera supplying the view and projection for this draw.
	void TileMapRenderer::Render(Core::IRenderer* renderer, Core::ICamera* camInst)
	{
	}

	/// @brief Builds the tile-map data and geometry used for rendering.
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

	/// @brief Copies this object's state into the requested target or attaches its clone to the target actor.
	/// @param actor Actor participating in this operation.
	void TileMapRenderer::CloneTo(IActor* actor)
	{
		TileMapRenderer* newComp = DBG_NEW TileMapRenderer(actor, GetUpdateOrder());
		newComp->mTileMapKey	 = this->mTileMapKey;
	}

	/// @brief Initializes tile-map bindings for its owning actor.
	/// @param owner Actor or object associated with the new instance.
	/// @param updateOrder Order used when dispatching component updates.
	/// @note Initializes the :TileMapRenderer base or delegates to its constructor.
	TileMapRenderer::TileMapRenderer(IActor* owner, int updateOrder)
		: SpriteRenderer(owner, updateOrder)
		, mTileMap(nullptr)
		, mTileMapKey(Common::ChunkKey::NullVal::NULL_OBJECT)

	{
	}

	/// @brief Completes destruction through the object's inheritance hierarchy.
	TileMapRenderer::~TileMapRenderer()
	{
		// ResourceManager::GetInstance()->RemoveLoadedMeshes(GetMeshKey());
		mTileMap = nullptr;
	}

	/// @brief Serializes this object's persistent properties to a .chunk stream.
	/// @param ofs Output stream receiving the serialized data.
	/// @note Writes to the supplied stream at its current position.
	void TileMapRenderer::SaveProperties(std::ofstream& ofs)
	{
		D3D11Component::SaveProperties(ofs);
		FileIOHelper::SaveString(ofs, ChunkKey::TileMapRenderer::TILEMAP_KEY, mTileMap->GetFileName());
	}

	/// @brief Restores this object's persistent properties from a .chunk stream.
	/// @param ifs Input stream positioned at the expected data; reading advances its position.
	/// @note Advances the stream position and updates the destination state.
	void TileMapRenderer::LoadProperties(std::ifstream& ifs)
	{
		Common::FTDS::String texKey;
		FileIOHelper::LoadBasicString(ifs, texKey);
		// SetTexKey(texKey);
		FileIOHelper::LoadBasicString(ifs, mTileMapKey);
		D3D11Component::LoadProperties(ifs);
	}

#ifdef FOXTROT_EDITOR
	/// @brief Provides an empty lifecycle or extension hook for this implementation.
	/// @param deltaTime Elapsed frame time in seconds.
	void TileMapRenderer::EditorUpdate(float deltaTime)
	{
	}
	/// @brief Provides an empty lifecycle or extension hook for this implementation.
	/// @param renderer Renderer providing the graphics device and current render state.
	/// @param camInst Camera supplying the view and projection for this draw.
	void TileMapRenderer::EditorRender(Core::IRenderer* renderer, Core::ICamera* camInst)
	{
	}
	/// @brief Updates the object's editor-facing controls.
	/// @param chInst Glyph instance whose metrics or vertices are used.
	void TileMapRenderer::EditorUIUpdate(Editor::CommandHistory* chInst)
	{
		// UpdateSprite();
		UpdateCSV();

		if (ImGui::Button("Update"))
			this->InitializeTileMap();
	}

	/// @brief Refreshes tile-map data from its CSV resource.
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
				"SelectCSV", "Select .CSV", Common::FileTypes::TEXTURE, config);
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
	/// @brief Refreshes tile-map data from its CSV resource.
	/// @param key Lookup key identifying the stored entry.
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
				"SelectCSV", "Select .CSV", Common::FileTypes::TEXTURE, config);
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
namespace D3D11 {
/// @brief Creates a tile-map-rendering component associated with an actor.
/// @param actor Actor participating in this operation.
/// @return Created tile map renderer instance or resource.
D3D11_API D3D11::TileMapRenderer* CreateTileMapRenderer(Core::IActor* actor)
	{
		return DBG_NEW D3D11::TileMapRenderer(actor);
	}
}
