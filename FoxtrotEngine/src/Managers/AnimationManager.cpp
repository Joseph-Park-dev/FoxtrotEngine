#include "AnimationManager.h"

#include <Windows.h>

#include "ResourceSystem/Animation/FTSpriteAnimation.h"
#include "ResourceSystem/FTTileMap.h"
#include "ResourceSystem/GeometryGenerator.h"
#include "Managers/ResourceManager.h"
#include "Core/FTCore.h"
#include "FileSystem/NullKeys.h"
#include "Renderer/FoxtrotRenderer.h"

#ifdef FOXTROT_EDITOR
	#include "EditorUtils.h"
	#include "FileSystem/BufferSizes.h"
#endif // FOXTROT_EDITOR

FTSpriteAnimation* AnimationManager::CreateAnimationFromTile(const char* name, UINT texKey, UINT tileMapKey)
{
	if (!mRenderer)
		printf("ERROR : Animator::CreateAnimationFromTile()-> Renderer is null");

	FTSpriteAnimation* animation= DBG_NEW FTSpriteAnimation;
	std::string	animName = std::string(name) + FileTypes::SPRITE_ANIMATION;
	animation->SetFileName(animName);

	std::string path = ResourceManager::GetInstance()->GetPathToAsset().append(animName);
	animation->SetRelativePath(path);

	if (texKey != ChunkKey::NullVal::VALUE_NOT_ASSIGNED)
		animation->SetTexture(texKey);
	if (tileMapKey != ChunkKey::NullVal::VALUE_NOT_ASSIGNED)
		animation->SetTileMapKey(tileMapKey);

	FTTileMap* tileMapBuf = ResourceManager::GetInstance()->GetLoadedTileMap(tileMapKey);
	if (tileMapBuf->GetTiles() == nullptr)
		tileMapBuf->ReadCSV();

	std::vector<FTMeshData> meshDataBuf;
	GeometryGenerator::MakeSpriteAnimation(
		meshDataBuf, tileMapBuf->GetTiles(), tileMapBuf->GetMaxCountOnMapX(), tileMapBuf->GetMaxCountOnMapY());
	animation->Initialize(std::move(meshDataBuf), mRenderer->GetDevice(), mRenderer->GetContext());
	printf("FTSpriteAnimation created, %s\n", name);

	return animation;
}

void AnimationManager::Initialize(FoxtrotRenderer* renderer)
{
	mRenderer = renderer;
}

// FTSpriteAnimation* AnimationManager::CopySpriteAnimation(FTSpriteAnimation* spriteAnim)
//{
//	//if (!mRenderer)
//	//	printf("ERROR : Animator::CreateAnimationFromTile()-> Renderer is null");
//
//	//FTSpriteAnimation* newAnim = DBG_NEW FTSpriteAnimation(spriteAnim);
//	//newAnim->SetTexture();
//
//	//FTTileMap* tileMapBuf = ResourceManager::GetInstance()->GetLoadedTileMap(tileMapKey);
//	//if (tileMapBuf)
//	//	tileMapBuf->ReadCSV();
//
//	//std::vector<FTMeshData> meshDataBuf;
//	//GeometryGenerator::MakeSpriteAnimation(
//	//	meshDataBuf, tileMapBuf->GetTiles(), tileMapBuf->GetMaxCountOnMapX(), tileMapBuf->GetMaxCountOnMapY());
//	//newAnim->Initialize(meshDataBuf, mRenderer->GetDevice(), mRenderer->GetContext());
//
//	//printf("FTSpriteAnimation created, %s", name);
//
//	//return animation;
//	return nullptr;
// }

AnimationManager::AnimationManager()
{
}

AnimationManager::~AnimationManager()
{
}

#ifdef FOXTROT_EDITOR
void AnimationManager::UpdateUI(bool* opened)
{
	if (!ImGui::Begin("Animation Manager", opened))
	{
		ImGui::End();
	}
	else
	{
		CreateAnimation();

		std::unordered_map<UINT, FTSpriteAnimation*>& map = 
			ResourceManager::GetInstance()->GetSpriteAnimMap();
		std::unordered_map<UINT, FTSpriteAnimation*>::iterator iter = map.begin();

		for (; iter != map.end(); ++iter)
		{
			if ((*iter).second)
			{
				(*iter).second->UpdateUI();
				if (ImGui::Button("Save"))
					SaveSpriteAnimAsFile((*iter).second, (*iter).first);
			}
		}

		ImGui::End();
	}
}

void AnimationManager::CreateAnimation()
{
	if (ImGui::Button("Create Sprite Animation"))
	{
		ImGui::OpenPopup("CreateSpriteAnimation");
	}
	if (ImGui::BeginPopupModal("CreateSpriteAnimation"))
	{
		static char name[BufferSize::STRING_BUFFER_SIZE] = "Empty Value";
		ImGui::InputText("Name", name, BufferSize::STRING_BUFFER_SIZE);

		const char* text = ChunkKey::NullVal::NULL_OBJ;

		static UINT texKey = ChunkKey::NullVal::VALUE_NOT_ASSIGNED;
		GetSprite(texKey);
		if (texKey != ChunkKey::NullVal::VALUE_NOT_ASSIGNED)
			text = ResourceManager::GetInstance()->GetLoadedTexture(texKey)->GetFileName().c_str();

		ImGui::Text(text);

		text				   = ChunkKey::NullVal::NULL_OBJ;
		static UINT tileMapKey = ChunkKey::NullVal::VALUE_NOT_ASSIGNED;
		GetTileMap(tileMapKey);
		if (tileMapKey != ChunkKey::NullVal::VALUE_NOT_ASSIGNED)
			text = ResourceManager::GetInstance()->GetLoadedTileMap(tileMapKey)->GetFileName().c_str();

		ImGui::Text(text);

		if (ImGui::Button("Create"))
		{
			FTSpriteAnimation* anim = CreateAnimationFromTile(name, texKey, tileMapKey);

			// Load the created animation to ResourceManager & File.
			// This is called only during the FTEditor Runtime.
			UINT key =
				ResourceManager::GetInstance()->LoadResource(
					anim, ResourceManager::GetInstance()->GetSpriteAnimMap());
			SaveSpriteAnimAsFile(anim, key);
		}

		if (ImGui::Button("Close"))
			ImGui::CloseCurrentPopup();
		ImGui::Separator();
		ImGui::EndPopup();
	}
}

void AnimationManager::GetSprite(UINT& key)
{
	FTEditorUtils::DisplayResSelection("Select Sprite", ResourceManager::GetInstance()->GetTexturesMap(), key);
}

void AnimationManager::GetTileMap(UINT& key)
{
	FTEditorUtils::DisplayResSelection("Select TileMap", ResourceManager::GetInstance()->GetTileMapsMap(), key);
}

void AnimationManager::SaveSpriteAnimAsFile(FTSpriteAnimation* animation, UINT key)
{
	std::ofstream ofs(animation->GetRelativePath());
	animation->SaveProperties(ofs, key);
	FileIOHelper::SaveBufferToFile(ofs);
}
#endif // FOXTROT_EDITOR
