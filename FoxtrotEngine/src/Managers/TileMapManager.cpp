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
	spriteSheet->FileName().Assign(spriteSheet->FileName());
	spriteSheet->FileName().Assign(FileTypes::TILEMAP);
	FTDS::String fullPath = FTDS::String(path) + spriteSheet->FileName();

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

		FTDS::HashChainMap<FTTileMap*>* mapTileMaps = EditorResourceManager::GetInstance()->GetTileMaps();
		auto iterT = mapTileMaps->Begin();

		for (; iterT != mapTileMaps->End(); ++iterT)
		{
			if ((*iterT)->Value())
			{
				ImGui::PushID((*iterT)->Value());
				(*iterT)->Value()->UpdateUI();
				if (ImGui::Button("Save TileMap"))
					SaveTileMapAsFile((*iterT)->Value());
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

		FTDS::HashChainMap<FTSpriteSheet*>* mapSpriteSheets = EditorResourceManager::GetInstance()->GetSpriteSheets();
		auto iterS = mapSpriteSheets->Begin();

		for (; iterS != mapSpriteSheets->End(); ++iterS)
		{
			if ((*iterS)->Value())
			{
				ImGui::PushID((*iterS)->Value());
				(*iterS)->Value()->UpdateUI();
				if (ImGui::Button("Save SpriteSheet"))
				{
					(*iterS)->Value()->Initialize();
					SaveSpriteSheetAsFile((*iterS)->Value());
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