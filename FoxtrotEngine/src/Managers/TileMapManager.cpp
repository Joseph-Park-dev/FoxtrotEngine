#include "TileMapManager.h"

#include <fstream>

#include "ResourceSystem/FTTileMap.h"
#include "FileSystem/FileIOHelper.h"
#include "Managers/ResourceManager.h"

#ifdef FOXTROT_EDITOR
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#endif // FOXTROT_EDITOR

TileMapManager::TileMapManager()
{ }

TileMapManager::~TileMapManager()
{
}

void TileMapManager::SaveTileMapAsFile(FTTileMap* tileMap, UINT key)
{
	const std::string& path = tileMap->GetRelativePath();
	tileMap->SetFileName(tileMap->GetFileName() + FileTypes::TILEMAP);
	const std::string& name = tileMap->GetFileName();
	const std::string fullPath = path + name;

	tileMap->SetRelativePath(fullPath);
	std::ofstream ofs(fullPath);
	tileMap->SaveProperties(ofs, key);
	FileIOHelper::SaveBufferToFile(ofs);
}

void TileMapManager::UpdateUI(bool* opened)
{
	if (!ImGui::Begin("TileMap Manager", opened))
	{
		ImGui::End();
	}
	else
	{
		ImGui::Text("Loaded TileMaps");

		std::unordered_map<UINT, FTTileMap*>& map = ResourceManager::GetInstance()->GetTileMapsMap();
		std::unordered_map<UINT, FTTileMap*>::iterator iter = map.begin();

		for (; iter != map.end(); ++iter)
		{
			if ((*iter).second)
			{
				(*iter).second->UpdateUI();
				if (ImGui::Button("Save"))
					SaveTileMapAsFile((*iter).second, (*iter).first);
			}
		}

		if (ImGui::Button("Create"))
		{
			FTTileMap* tileMap = DBG_NEW FTTileMap;
			tileMap->SetRelativePath(ResourceManager::GetInstance()->GetPathToAsset());
			ResourceManager::GetInstance()->LoadResource(tileMap, map);
		}

		ImGui::End();
	}
}
