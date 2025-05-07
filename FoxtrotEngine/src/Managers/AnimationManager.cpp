#include "AnimationManager.h"

#include <Windows.h>
#include "spine/spine.h"

#include "ResourceSystem/Animation/FTSpriteAnimation.h"
#include "ResourceSystem/FTTileMap.h"
#include "ResourceSystem/FTSpriteSheet.h"
#include "ResourceSystem/GeometryGenerator.h"
#include "Managers/ResourceManager.h"
#include "Core/FTCore.h"
#include "FileSystem/NullKeys.h"
#include "Renderer/FoxtrotRenderer.h"

#ifdef FOXTROT_EDITOR
	#include "EditorUtils.h"
	#include "FileSystem/BufferSizes.h"
	#include "EditorResourceManager.h"
#endif // FOXTROT_EDITOR
//
// FTSpriteAnimation* AnimationManager::CreateAnimationFromTileMap(const char* name, UINT texKey, UINT tileMapKey)
//{
//	if (!mRenderer)
//		printf("ERROR : Animator::CreateAnimationFromTile()-> Renderer is null");
//
//	FTSpriteAnimation* animation= DBG_NEW FTSpriteAnimation;
//	std::string	animName = std::string(name) + FileTypes::SPRITE_ANIMATION;
//	animation->SetFileName(animName);
//
//	std::string path = ResourceManager::GetInstance()->GetPathToAsset().append(animName);
//	animation->SetRelativePath(path);
//
//	if (texKey != ChunkKey::NullVal::VALUE_NOT_ASSIGNED)
//		animation->SetTexture(texKey);
//	if (tileMapKey != ChunkKey::NullVal::VALUE_NOT_ASSIGNED)
//		animation->SetTileDataKey(tileMapKey);
//
//	FTTileMap* tileMapBuf = ResourceManager::GetInstance()->GetLoadedTileMap(tileMapKey);
//	if (tileMapBuf->GetTiles() == nullptr)
//		tileMapBuf->Initialize();
//
//	std::vector<FTMeshData> meshDataBuf;
//	GeometryGenerator::MakeSpriteAnimation(
//		meshDataBuf, tileMapBuf->GetTiles(), tileMapBuf->GetMaxCountOnMapX(), tileMapBuf->GetMaxCountOnMapY());
//	animation->Initialize(std::move(meshDataBuf), mRenderer->GetDevice(), mRenderer->GetContext());
//	printf("FTSpriteAnimation created, %s\n", name);
//
//	return animation;
//}

FTSpriteAnimation* AnimationManager::CreateAnimationFromSpriteSheet(const char* name, UINT texKey, UINT spriteSheetKey, size_t startIndex, size_t endIndex)
{
	if (!mRenderer)
		printf("ERROR : Animator::CreateAnimationFromTile()-> Renderer is null");

	FTSpriteAnimation* animation = DBG_NEW FTSpriteAnimation;
	std::string	animName = std::string(name) + FileTypes::SPRITE_ANIMATION;
	animation->SetFileName(animName);

	std::string path = ResourceManager::GetInstance()->GetPathToAsset().append(animName);
	animation->SetRelativePath(path);

	if (texKey != ChunkKey::NullVal::VALUE_NOT_ASSIGNED)
		animation->SetTexture(texKey);
	if (spriteSheetKey != ChunkKey::NullVal::VALUE_NOT_ASSIGNED)
		animation->SetTileDataKey(spriteSheetKey);

#ifdef FOXTROT_EDITOR
	FTSpriteSheet* spriteSheetBuf = EditorResourceManager::GetInstance()->GetLoadedSpriteSheet(spriteSheetKey);
#else
	FTSpriteSheet* spriteSheetBuf = ResourceManager::GetInstance()->GetLoadedSpriteSheet(spriteSheetKey);
#endif // FOXTROT_EDITOR

	if (spriteSheetBuf->GetTiles() == nullptr)
		spriteSheetBuf->Initialize();

	std::vector<FTMeshData> meshDataBuf;
	GeometryGenerator::MakeSpriteAnimation(
		meshDataBuf, spriteSheetBuf->GetTiles(), startIndex, endIndex);
	animation->Initialize(std::move(meshDataBuf), mRenderer->GetDevice(), mRenderer->GetContext());
	printf("FTSpriteAnimation created, %s\n", name);

	return animation;
}

spine::SkeletonRenderer* AnimationManager::GetSkeletonRenderer()
{
	return mSkeletonRenderer;
}

void AnimationManager::Initialize(FoxtrotRenderer* renderer)
{
	mRenderer		  = renderer;
	mSkeletonRenderer = new spine::SkeletonRenderer;
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
	: mRenderer(nullptr)
	, mSkeletonRenderer(nullptr)
{
}

AnimationManager::~AnimationManager()
{
	delete mSkeletonRenderer;
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
			EditorResourceManager::GetInstance()->GetSpriteAnimMap();
		std::unordered_map<UINT, FTSpriteAnimation*>::iterator iter = map.begin();

		if (ImGui::TreeNode("Loaded Animations"))
		{
			for (; iter != map.end(); ++iter)
			{
				if ((*iter).second)
				{
					if (ImGui::BeginListBox((*iter).second->GetFileName().c_str(), ImVec2(-FLT_MIN, 100)))
					{
						ImGui::Text((*iter).second->GetFileName().c_str());
						(*iter).second->UpdateUI();
						if (ImGui::Button("Save"))
							SaveSpriteAnimAsFile((*iter).second, (*iter).first);

						ImGui::EndListBox();
					}
				}
			}
			ImGui::TreePop();
		}

		ImGui::End();
	}
}

void AnimationManager::CreateAnimation()
{
	if (ImGui::Button("Create Animation from SpriteSheet"))
	{
		ImGui::OpenPopup("CreateSpriteSheetAnim");
	}
	if (ImGui::BeginPopupModal("CreateSpriteSheetAnim"))
	{
		static char name[BufferSize::STRING_BUFFER_SIZE] = "Empty Value";
		ImGui::InputText("Name", name, BufferSize::STRING_BUFFER_SIZE);

		const char* text = ChunkKey::NullVal::NULL_OBJ;

		static UINT texKey = ChunkKey::NullVal::VALUE_NOT_ASSIGNED;
		GetSprite(texKey);
		if (texKey != ChunkKey::NullVal::VALUE_NOT_ASSIGNED)
			text = EditorResourceManager::GetInstance()->GetLoadedTexture(texKey)->GetFileName().c_str();

		ImGui::Text(text);

		text					   = ChunkKey::NullVal::NULL_OBJ;
		static UINT spriteSheetKey = ChunkKey::NullVal::VALUE_NOT_ASSIGNED;
		GetSpriteSheet(spriteSheetKey);
		if (spriteSheetKey != ChunkKey::NullVal::VALUE_NOT_ASSIGNED)
			text = EditorResourceManager::GetInstance()->GetLoadedSpriteSheet(spriteSheetKey)->GetFileName().c_str();

		static int startIdx;
		static int endIdx;

		CommandHistory::GetInstance()->UpdateIntValue("Anim Start Index", startIdx);
		CommandHistory::GetInstance()->UpdateIntValue("Anim End Index", endIdx);

		ImGui::Text(text);

		if (ImGui::Button("Create"))
		{
			FTSpriteAnimation* anim = CreateAnimationFromSpriteSheet(name, texKey, spriteSheetKey, startIdx, endIdx);

			// Load the created animation to ResourceManager & File.
			// This is called only during the FTEditor Runtime.
			UINT key =
				EditorResourceManager::GetInstance()->LoadResource(
					anim, EditorResourceManager::GetInstance()->GetSpriteAnimMap());
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
	FTEditorUtils::DisplayResSelection("Select Sprite", EditorResourceManager::GetInstance()->GetTexturesMap(), key);
}

void AnimationManager::GetTileMap(UINT& key)
{
	FTEditorUtils::DisplayResSelection("Select TileMap", EditorResourceManager::GetInstance()->GetTileMapsMap(), key);
}

void AnimationManager::GetSpriteSheet(UINT& key)
{
	FTEditorUtils::DisplayResSelection("Select SpriteSheet", EditorResourceManager::GetInstance()->GetSpriteSheetsMap(), key);
}

void AnimationManager::SaveSpriteAnimAsFile(FTSpriteAnimation* animation, UINT key)
{
	std::string path = EditorResourceManager::GetInstance()->GetPathToAsset() + animation->GetFileName();
	std::ofstream ofs(path);
	animation->SaveProperties(ofs, key);
	FileIOHelper::SaveBufferToFile(ofs);
}
#endif // FOXTROT_EDITOR
