#include "TileMapManager.h"

#include <fstream>

#include "ResourceSystem/FTTileMap.h"
#include "ResourceSystem/FTSpriteSheet.h"
#include "FileSystem/FileIOHelper.h"
#include "Managers/ResourceManager.h"

#ifdef FOXTROT_EDITOR
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include "EditorResourceManager.h"
#endif // FOXTROT_EDITOR

TileMapManager::TileMapManager()
{ }

TileMapManager::~TileMapManager()
{
}

void TileMapManager::SaveTileMapAsFile(FTTileMap* tileMap)
{
	const std::string& path = tileMap->GetRelativePath();
	tileMap->SetFileName(tileMap->FileName() + FileTypes::TILEMAP);
	const std::string& name = tileMap->FileName();
	const std::string fullPath = path + name;

	tileMap->SetRelativePath(fullPath);
	std::ofstream ofs(fullPath);
	tileMap->SaveProperties(ofs);
	FileIOHelper::SaveBufferToFile(ofs);
}

void TileMapManager::SaveSpriteSheetAsFile(FTSpriteSheet* spriteSheet)
{
	const std::string& path = spriteSheet->GetRelativePath();
	spriteSheet->SetFileName(spriteSheet->FileName() + FileTypes::SPRITE_SHEET);
	const std::string& name = spriteSheet->FileName();
	const std::string fullPath = path + name;

	spriteSheet->SetRelativePath(fullPath);
	std::ofstream ofs(fullPath);
	spriteSheet->SaveProperties(ofs);
	FileIOHelper::SaveBufferToFile(ofs);
}

#ifdef FOXTROT_EDITOR
void TileMapManager::UpdateUI(bool* opened)
{
	if (!ImGui::Begin("TileMap Manager", opened))
	{
		ImGui::End();
	}
	else
	{
		ImGui::Text("Loaded TileMaps");

		FTDS::HashChainMap<FTDS::String, FTTileMap*>& mapTileMaps = EditorResourceManager::GetInstance()->GetTileMapsMap();
		FTDS::HashChainMap<FTDS::String, FTTileMap*>::iterator iterT = mapTileMaps.begin();

		for (; iterT != mapTileMaps.end(); ++iterT)
		{
			if ((*iterT).second)
			{
				ImGui::PushID((*iterT).second);
				(*iterT).second->UpdateUI();
				if (ImGui::Button("Save TileMap"))
					SaveTileMapAsFile((*iterT).second);
				ImGui::PopID();
			}
		}

		if (ImGui::Button("Create TileMap"))
		{
			FTTileMap* tileMap = DBG_NEW FTTileMap;
			tileMap->SetRelativePath(EditorResourceManager::GetInstance()->GetPathToAsset());
			EditorResourceManager::GetInstance()->LoadResource(tileMap, mapTileMaps);
		}

		ImGui::SeparatorText("Loaded SpriteSheets");

		FTDS::HashChainMap<FTDS::String, FTSpriteSheet*>& mapSpriteSheets = EditorResourceManager::GetInstance()->GetSpriteSheetsMap();
		FTDS::HashChainMap<FTDS::String, FTSpriteSheet*>::iterator iterS = mapSpriteSheets.begin();

		for (; iterS != mapSpriteSheets.end(); ++iterS)
		{
			if ((*iterS).second)
			{
				ImGui::PushID((*iterS).second);
				(*iterS).second->UpdateUI();
				if (ImGui::Button("Save SpriteSheet"))
				{
					(*iterS).second->Initialize();
					SaveSpriteSheetAsFile((*iterS).second);
				}
				ImGui::PopID();
			}
		}

		if (ImGui::Button("Create SpriteSheet"))
		{
			FTSpriteSheet* spriteSheet = DBG_NEW FTSpriteSheet;
			spriteSheet->SetRelativePath(EditorResourceManager::GetInstance()->GetPathToAsset());
			EditorResourceManager::GetInstance()->LoadResource(spriteSheet, mapSpriteSheets);
		}

		ImGui::End();
	}
}
#endif // FOXTROT_EDITOR