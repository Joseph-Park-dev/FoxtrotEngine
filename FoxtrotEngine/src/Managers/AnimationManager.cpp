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
//	if (texKey != ChunkKey::NullVal::NULL_OBJECT)
//		animation->SetTexture(texKey);
//	if (tileMapKey != ChunkKey::NullVal::NULL_OBJECT)
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

FTSpriteAnimation* AnimationManager::CreateAnimationFromSpriteSheet(const char* name, const char* texKey, const char* spriteSheetKey, size_t startIndex, size_t endIndex)
{
	if (!mRenderer)
		printf("ERROR : Animator::CreateAnimationFromTile()-> Renderer is null");

	FTSpriteAnimation* animation					= DBG_NEW FTSpriteAnimation;
	std::string							   animName = std::string(name) + FileTypes::SPRITE_ANIMATION;
	animation->SetFileName(animName);

	std::string path = ResourceManager::GetInstance()->GetPathToAsset().append(animName);
	animation->SetRelativePath(path);

	if (!FTDS::StringEqual(texKey, ChunkKey::NullVal::NULL_OBJECT))
		animation->SetTexture(texKey);
	if (!FTDS::StringEqual(spriteSheetKey, ChunkKey::NullVal::NULL_OBJECT))
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

#ifdef FOXTROT_EDITOR
	EditorResourceManager::GetInstance()->LoadResource(
		animation, EditorResourceManager::GetInstance()->GetSpriteAnimMap());
#else
	ResourceManager::GetInstance()->LoadResource(
		animation, EditorResourceManager::GetInstance()->GetSpriteAnimMap());
#endif // FOXTROT_EDITOR

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

		std::unordered_map<const char*, FTSpriteAnimation*>& map =
			EditorResourceManager::GetInstance()->GetSpriteAnimMap();
		std::unordered_map<const char*, FTSpriteAnimation*>::iterator iter = map.begin();

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
							SaveSpriteAnimAsFile((*iter).second);

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

		const char* text = ChunkKey::NullVal::NULL_OBJECT;

		static const char* texKey = ChunkKey::NullVal::NULL_OBJECT;
		GetSprite(texKey);
		if (!FTDS::StringEqual(texKey, ChunkKey::NullVal::NULL_OBJECT))
			text = EditorResourceManager::GetInstance()->GetLoadedTexture(texKey)->GetFileName().c_str();

		ImGui::Text(text);

		text							  = ChunkKey::NullVal::NULL_OBJECT;
		static const char* spriteSheetKey = ChunkKey::NullVal::NULL_OBJECT;
		GetSpriteSheet(spriteSheetKey);
		FTSpriteSheet* spriteSheet = nullptr;
		int			   maxIdx	   = 0;
		if (!FTDS::StringEqual(spriteSheetKey, ChunkKey::NullVal::NULL_OBJECT))
		{
			text		= EditorResourceManager::GetInstance()->GetLoadedSpriteSheet(spriteSheetKey)->GetFileName().c_str();
			spriteSheet = EditorResourceManager::GetInstance()->GetLoadedSpriteSheet(spriteSheetKey);
			maxIdx		= static_cast<int>(spriteSheet->GetTilesCount()) - 1;
		}
		static int startIdx;
		static int endIdx;

		CommandHistory::GetInstance()->UpdateIntValue("Anim Start Index", startIdx, -maxIdx, maxIdx);
		CommandHistory::GetInstance()->UpdateIntValue("Anim End Index", endIdx, -maxIdx, maxIdx);
		if (ImGui::Button("Auto Detect"))
			startIdx = 0;
		endIdx = maxIdx;

		ImGui::Text(text);

		if (ImGui::Button("Create"))
		{
			FTSpriteAnimation* anim = CreateAnimationFromSpriteSheet(name, texKey, spriteSheetKey, startIdx, endIdx);

			// Load the created animation to ResourceManager & File.
			// This is called only during the FTEditor Runtime.
			SaveSpriteAnimAsFile(anim);
		}

		if (ImGui::Button("Close"))
			ImGui::CloseCurrentPopup();
		ImGui::Separator();
		ImGui::EndPopup();
	}
}

void AnimationManager::GetSprite(const char*& key)
{
	FTEditorUtils::DisplayResSelection("Select Sprite", EditorResourceManager::GetInstance()->GetTexturesMap(), key);
}

void AnimationManager::GetTileMap(const char*& key)
{
	FTEditorUtils::DisplayResSelection("Select TileMap", EditorResourceManager::GetInstance()->GetTileMapsMap(), key);
}

void AnimationManager::GetSpriteSheet(const char*& key)
{
	FTEditorUtils::DisplayResSelection("Select SpriteSheet", EditorResourceManager::GetInstance()->GetSpriteSheetsMap(), key);
}

void AnimationManager::SaveSpriteAnimAsFile(FTSpriteAnimation* animation)
{
	std::string	  path = EditorResourceManager::GetInstance()->GetPathToAsset() + animation->GetFileName();
	std::ofstream ofs(path);
	animation->SaveProperties(ofs);
	FileIOHelper::SaveBufferToFile(ofs);
}
#endif // FOXTROT_EDITOR
