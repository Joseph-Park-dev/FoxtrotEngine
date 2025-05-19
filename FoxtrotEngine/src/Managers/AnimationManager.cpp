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

#include "Static/FTString.h"

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
//	FTDS::String	animName = FTDS::String(name) + FileTypes::SPRITE_ANIMATION;
//	animation->SetFileName(animName);
//
//	FTDS::String path = ResourceManager::GetInstance()->GetPathToAsset().append(animName);
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

FTSpriteAnimation* AnimationManager::CreateAnimationFromSpriteSheet(const char* name, FTDS::String& texKey, FTDS::String& spriteSheetKey, size_t startIndex, size_t endIndex)
{
	if (!mRenderer)
		printf("ERROR : Animator::CreateAnimationFromTile()-> Renderer is null");

	FTSpriteAnimation* animation					= DBG_NEW FTSpriteAnimation;
	FTDS::String						   animName = FTDS::String(name) + FileTypes::SPRITE_ANIMATION;
	animation->SetFileName(animName);

	FTDS::String path = ResourceManager::GetInstance()->GetPathToAsset();
	path.Append(animName);
	animation->SetRelativePath(path);

	if (texKey.NotEqual(ChunkKey::NullVal::NULL_OBJECT))
		animation->SetTexture(texKey);
	if (spriteSheetKey.NotEqual(ChunkKey::NullVal::NULL_OBJECT))
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
		animation, EditorResourceManager::GetInstance()->GetSpriteAnimations());
#else
	ResourceManager::GetInstance()->LoadResource(
		animation, EditorResourceManager::GetInstance()->GetSpriteAnimations());
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

		FTDS::HashChainMap<FTSpriteAnimation*>* map =
			EditorResourceManager::GetInstance()->GetSpriteAnimations();
		auto iter = map->Begin();

		if (ImGui::TreeNode("Loaded Animations"))
		{
			for (; iter != map->End(); ++iter)
			{
				if ((*iter)->Value())
				{
					if (ImGui::BeginListBox((*iter)->Value()->FileName().C_Str(), ImVec2(-FLT_MIN, 100)))
					{
						ImGui::Text((*iter)->Value()->FileName().C_Str());
						(*iter)->Value()->UpdateUI();
						if (ImGui::Button("Save"))
							SaveSpriteAnimAsFile((*iter)->Value());

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

		static FTDS::String texKey = ChunkKey::NullVal::NULL_OBJECT;
		GetSprite(texKey);
		if (!FTDS::StringEqual(texKey.C_Str(), ChunkKey::NullVal::NULL_OBJECT))
			text = EditorResourceManager::GetInstance()->GetLoadedTexture(texKey.C_Str())->FileName().C_Str();

		ImGui::Text(text);

		text							   = ChunkKey::NullVal::NULL_OBJECT;
		static FTDS::String spriteSheetKey = ChunkKey::NullVal::NULL_OBJECT;
		GetSpriteSheet(spriteSheetKey);
		FTSpriteSheet* spriteSheet = nullptr;
		int			   maxIdx	   = 0;
		if (!FTDS::StringEqual(spriteSheetKey.C_Str(), ChunkKey::NullVal::NULL_OBJECT))
		{
			text		= EditorResourceManager::GetInstance()->GetLoadedSpriteSheet(spriteSheetKey.C_Str())->FileName().C_Str();
			spriteSheet = EditorResourceManager::GetInstance()->GetLoadedSpriteSheet(spriteSheetKey.C_Str());
			maxIdx		= static_cast<int>(spriteSheet->GetTilesCount()) - 1;
		}
		static int startIdx;
		static int endIdx;

		CommandHistory::GetInstance()->UpdateIntValue("Anim Start Index", startIdx, -maxIdx, maxIdx);
		CommandHistory::GetInstance()->UpdateIntValue("Anim End Index", endIdx, -maxIdx, maxIdx);
		if (ImGui::Button("Auto Detect"))
		{
			startIdx = 0;
			endIdx	 = maxIdx;
		}

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

void AnimationManager::GetSprite(FTDS::String& key)
{
	FTEditorUtils::DisplayResSelection("Select Sprite", EditorResourceManager::GetInstance()->GetTextures(), key);
}

void AnimationManager::GetTileMap(FTDS::String& key)
{
	FTEditorUtils::DisplayResSelection("Select TileMap", EditorResourceManager::GetInstance()->GetTileMaps(), key);
}

void AnimationManager::GetSpriteSheet(FTDS::String& key)
{
	FTEditorUtils::DisplayResSelection("Select SpriteSheet", EditorResourceManager::GetInstance()->GetSpriteSheets(), key);
}

void AnimationManager::SaveSpriteAnimAsFile(FTSpriteAnimation* animation)
{
	FTDS::String  path = EditorResourceManager::GetInstance()->GetPathToAsset() + animation->FileName();
	std::ofstream ofs(path.C_Str());
	animation->SaveProperties(ofs);
	FileIOHelper::SaveBufferToFile(ofs);
}
#endif // FOXTROT_EDITOR
