#include "TileMapManager.h"

#include <fstream>

#include "ResourceSystem/FTTileMap.h"
#include "ResourceSystem/FTSpriteSheet.h"
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

void TileMapManager::SaveSpriteSheetAsFile(FTSpriteSheet* spriteSheet, UINT key)
{
	const std::string& path = spriteSheet->GetRelativePath();
	spriteSheet->SetFileName(spriteSheet->GetFileName() + FileTypes::SPRITE_SHEET);
	const std::string& name = spriteSheet->GetFileName();
	const std::string fullPath = path + name;

	spriteSheet->SetRelativePath(fullPath);
	std::ofstream ofs(fullPath);
	spriteSheet->SaveProperties(ofs, key);
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

		std::unordered_map<UINT, FTTileMap*>& mapTileMaps = ResourceManager::GetInstance()->GetTileMapsMap();
		std::unordered_map<UINT, FTTileMap*>::iterator iterT = mapTileMaps.begin();

		for (; iterT != mapTileMaps.end(); ++iterT)
		{
			if ((*iterT).second)
			{
				ImGui::PushID((*iterT).second);
				(*iterT).second->UpdateUI();
				if (ImGui::Button("Save TileMap"))
					SaveTileMapAsFile((*iterT).second, (*iterT).first);
				ImGui::PopID();
			}
		}

		if (ImGui::Button("Create TileMap"))
		{
			FTTileMap* tileMap = DBG_NEW FTTileMap;
			tileMap->SetRelativePath(ResourceManager::GetInstance()->GetPathToAsset());
			ResourceManager::GetInstance()->LoadResource(tileMap, mapTileMaps);
		}

		ImGui::SeparatorText("Loaded SpriteSheets");

		std::unordered_map<UINT, FTSpriteSheet*>& mapSpriteSheets = ResourceManager::GetInstance()->GetSpriteSheetsMap();
		std::unordered_map<UINT, FTSpriteSheet*>::iterator iterS = mapSpriteSheets.begin();

		for (; iterS != mapSpriteSheets.end(); ++iterS)
		{
			if ((*iterS).second)
			{
				ImGui::PushID((*iterS).second);
				(*iterS).second->UpdateUI();
				if (ImGui::Button("Save SpriteSheet"))
				{
					SaveSpriteSheetAsFile((*iterS).second, (*iterS).first);
					(*iterS).second->Initialize();
				}
				ImGui::PopID();
			}
		}

		if (ImGui::Button("Create SpriteSheet"))
		{
			FTSpriteSheet* spriteSheet = DBG_NEW FTSpriteSheet;
			spriteSheet->SetRelativePath(ResourceManager::GetInstance()->GetPathToAsset());
			ResourceManager::GetInstance()->LoadResource(spriteSheet, mapSpriteSheets);
		}

		ImGui::End();
	}
}
