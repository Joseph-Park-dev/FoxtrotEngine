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
	const char* path = tileMap->RelativePath().C_Str();
	tileMap->FileName().Assign(tileMap->FileName());
	tileMap->FileName().Assign(FileTypes::TILEMAP);
	FTDS::String fullPath = FTDS::String(path) + tileMap->FileName();

	tileMap->SetRelativePath(fullPath);
	std::ofstream ofs(fullPath.C_Str());
	tileMap->SaveProperties(ofs);
	FileIOHelper::SaveBufferToFile(ofs);
}

void TileMapManager::SaveSpriteSheetAsFile(FTSpriteSheet* spriteSheet)
{
	const char* path = spriteSheet->RelativePath().C_Str();
	FTDS::String fullPath = FTDS::String(path) + spriteSheet->FileName() + FileTypes::SPRITE_SHEET;

	spriteSheet->SetRelativePath(fullPath);
	std::ofstream ofs(fullPath.C_Str());
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

		FTDS::HashMap<FTTileMap*>* mapTileMaps = EditorResourceManager::GetInstance()->GetTileMaps();

		mapTileMaps->IterateAllValues([&](FTTileMap* tileMap) {
			if (tileMap)
			{
				ImGui::PushID(tileMap);
				tileMap->UpdateUI();
				if (ImGui::Button("Save TileMap"))
					SaveTileMapAsFile(tileMap);
				ImGui::PopID();
			}
		});

		if (ImGui::Button("Create TileMap"))
		{
			FTTileMap* tileMap = DBG_NEW FTTileMap;
			tileMap->SetFileName("New TileMap");
			tileMap->SetRelativePath(EditorResourceManager::GetInstance()->GetPathToAsset());
			EditorResourceManager::GetInstance()->LoadResource(tileMap, mapTileMaps);
		}

		ImGui::SeparatorText("Loaded SpriteSheets");

		FTDS::HashMap<FTSpriteSheet*>* mapSpriteSheets = EditorResourceManager::GetInstance()->GetSpriteSheets();
		auto iterS = mapSpriteSheets->Begin();

		mapSpriteSheets->IterateAllValues(
			[&](FTSpriteSheet* res) {
				if (res)
				{
					ImGui::PushID(res);
					res->UpdateUI();
					if (ImGui::Button("Save SpriteSheet"))
					{
						res->Initialize();
						SaveSpriteSheetAsFile(res);
					}
					ImGui::Separator();
					ImGui::PopID();
				}
			});

		if (ImGui::Button("Create SpriteSheet"))
		{
			FTSpriteSheet* spriteSheet = DBG_NEW FTSpriteSheet;
			spriteSheet->SetFileName("New SpriteSheet");
			spriteSheet->SetRelativePath(ResourceManager::GetInstance()->GetPathToAsset());
			EditorResourceManager::GetInstance()->LoadResource(spriteSheet, mapSpriteSheets);
		}

		ImGui::End();
	}
}
#endif // FOXTROT_EDITOR