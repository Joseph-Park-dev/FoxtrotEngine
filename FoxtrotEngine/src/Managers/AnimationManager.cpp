#include "AnimationManager.h"

#include <Windows.h>
#include "spine/spine.h"

#include "ResourceSystem/Animation/FTSpriteAnimation.h"
#include "ResourceSystem/Animation/FTSpineAnimation.h"
#include "ResourceSystem/FTTileMap.h"
#include "ResourceSystem/FTSpriteSheet.h"
#include "ResourceSystem/GeometryGenerator.h"
#include "ResourceSystem/GenericData/FTText.h"
#include "ResourceSystem/GenericData/FTJSON.h"
#include "Managers/ResourceManager.h"
#include "Managers/FTSpineLoader.h"
#include "Managers/SpineExtension.h"
#include "Core/FTCore.h"
#include "FileSystem/NullKeys.h"
#include "Renderer/FoxtrotRenderer.h"

#include "Static/FTString.h"

#ifdef FOXTROT_EDITOR
	#include "EditorUtils.h"
	#include "FileSystem/BufferSizes.h"
	#include "EditorResourceManager.h"
#endif // FOXTROT_EDITOR

spine::FTSpineLoader* AnimationManager::GetSpineLoader()
{
	return mSpineLoader;
}

void AnimationManager::Initialize(FoxtrotRenderer* renderer)
{
	mRenderer = renderer;
}

AnimationManager::AnimationManager()
	: mRenderer(nullptr)
	, mSpineLoader(new spine::FTSpineLoader)
{
}

AnimationManager::~AnimationManager()
{
	delete mSpineLoader;
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

		FTDS::HashMap<FTSpriteAnimation*>* map =
			EditorResourceManager::GetInstance()->GetSpriteAnimations();
		auto iter = map->Begin();

		if (ImGui::TreeNode("Loaded Sprite Animations"))
		{
			map->IterateAllValues([&](FTSpriteAnimation* anim) {
				if (anim)
				{
					ImGui::PushID(anim->FileName().C_Str());
					if (ImGui::BeginListBox(anim->FileName().C_Str(), ImVec2(-FLT_MIN, 100)))
					{
						ImGui::Text(anim->FileName().C_Str());
						anim->UpdateUI();
						if (ImGui::Button("Save"))
							SaveAnimationAsFile(anim, FileTypes::SPRITE_ANIMATION);
						ImGui::EndListBox();
					}
					ImGui::PopID();
				}
			});
			ImGui::TreePop();
		}

		FTDS::HashMap<FTSpineAnimation*>* spineAnimMap =
			EditorResourceManager::GetInstance()->GetSpineAnimations();

		if (ImGui::TreeNode("Loaded Spine Animations"))
		{
			spineAnimMap->IterateAllValues([&](FTSpineAnimation* anim) {
				if (anim)
				{
					ImGui::PushID(anim->FileName().C_Str());
					if (ImGui::BeginListBox(anim->FileName().C_Str(), ImVec2(-FLT_MIN, 100)))
					{
						ImGui::Text(anim->FileName().C_Str());
						anim->UpdateUI();
						if (ImGui::Button("Save"))
							SaveAnimationAsFile(anim, FileTypes::SPINE_ANIMATION);
						ImGui::EndListBox();
					}
					ImGui::PopID();
				}
			});
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
		if (texKey.NotEqual(ChunkKey::NullVal::NULL_OBJECT))
			text = EditorResourceManager::GetInstance()->GetLoadedTexture(texKey)->FileName().C_Str();
		ImGui::Text(text);

		text							   = ChunkKey::NullVal::NULL_OBJECT;
		static FTDS::String spriteSheetKey = ChunkKey::NullVal::NULL_OBJECT;
		GetSpriteSheet(spriteSheetKey);
		FTSpriteSheet* spriteSheet = nullptr;
		int			   maxIdx	   = 0;
		if (spriteSheetKey.NotEqual(ChunkKey::NullVal::NULL_OBJECT))
		{
			text		= EditorResourceManager::GetInstance()->GetLoadedSpriteSheet(spriteSheetKey)->FileName().C_Str();
			spriteSheet = EditorResourceManager::GetInstance()->GetLoadedSpriteSheet(spriteSheetKey);
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
			SaveAnimationAsFile(anim, FileTypes::SPRITE_ANIMATION);
		}

		if (ImGui::Button("Close"))
			ImGui::CloseCurrentPopup();
		ImGui::Separator();
		ImGui::EndPopup();
	}

	if (ImGui::Button("Create Animation from Spine"))
	{
		ImGui::OpenPopup("CreateSpineAnim");
	}
	if (ImGui::BeginPopupModal("CreateSpineAnim"))
	{
		static char name[BufferSize::STRING_BUFFER_SIZE] = "Empty Value";
		ImGui::InputText("Name", name, BufferSize::STRING_BUFFER_SIZE);

		static FTDS::String jsonKey;
		FTEditorUtils::DisplayResSelection("Select Skeleton Data", EditorResourceManager::GetInstance()->GetJSONs(), jsonKey);

		static FTDS::String atlasKey;
		FTEditorUtils::DisplayResSelection("Select Spine Atlas", EditorResourceManager::GetInstance()->GetTexts(), atlasKey);

		if (ImGui::Button("Create"))
		{
			FTSpineAnimation* anim = CreateAnimationFromSpine(name, jsonKey, atlasKey);
			SaveAnimationAsFile(anim, FileTypes::SPINE_ANIMATION);
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

FTSpriteAnimation* AnimationManager::CreateAnimationFromSpriteSheet(const char* name, FTDS::String& texKey, FTDS::String& spriteSheetKey, size_t startIndex, size_t endIndex)
{
	if (!mRenderer)
		printf("ERROR : Animator::CreateAnimationFromTile()-> Renderer is null");

	FTSpriteAnimation* animation = DBG_NEW FTSpriteAnimation;

	FTDS::String animName = FTDS::String(name) + FileTypes::SPRITE_ANIMATION;
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
	ResourceManager::GetInstance()->GetSpriteAnimations()->Insert(animation->FileName(), animation);
	#endif // FOXTROT_EDITOR

	printf("FTSpriteAnimation created, %s\n", name);

	return animation;
}

FTSpineAnimation* AnimationManager::CreateAnimationFromSpine(const char* name, FTDS::String& jsonKey, FTDS::String& atlasKey)
{
	FTSpineAnimation* anim = DBG_NEW FTSpineAnimation;
	anim->SetJSONKey(jsonKey);
	anim->SetAtlasKey(atlasKey);

	FTJSON* json = EditorResourceManager::GetInstance()->GetLoadedJSON(jsonKey);
	FTText* text = EditorResourceManager::GetInstance()->GetLoadedText(atlasKey);
	EditorResourceManager::GetInstance()->RelativeToAbsolutePath(text);

	spine::Atlas		 atlas		  = spine::Atlas(text->RelativePath().C_Str(), mSpineLoader);
	spine::SkeletonData* skeletonData = mSpineLoader->ReadSkeletonJsonData(json->RelativePath().C_Str(), &atlas, 1.0f);

	// Configure mixing.
	// spine::AnimationStateData stateData(skeletonData);
	/*stateData.setMix("walk", "jump", 0.2f);
	stateData.setMix("jump", "run", 0.2f);*/

	anim->InitializeSpinAnim(mRenderer->GetDevice(), skeletonData);
	anim->SetTimeScale(1);
	// drawable.setUsePremultipliedAlpha(true);

	spine::Skeleton* skeleton = anim->GetSkeleton();
	skeleton->setToSetupPose();

	skeleton->setPosition(0.f, 0.f);
	skeleton->updateWorldTransform(spine::Physics_None);

	/*spine::Vector<spine::Animation*>& clips = skeletonData->getAnimations();
	for (size_t i = 0; i < clips.size(); ++i)
		anim->LoadedClips().add(clips[i]);*/

	anim->SetTexture((FTTexture*)atlas.getPages()[0]->texture);
	anim->SetFileName(name);

	#ifdef FOXTROT_EDITOR
	EditorResourceManager::GetInstance()->LoadResource(
		anim, EditorResourceManager::GetInstance()->GetSpineAnimations());
	#else
	ResourceManager::GetInstance()->GetSpineAnimations()->Insert(anim->FileName(), animation);
	#endif // FOXTROT_EDITOR

	delete skeletonData;
	return anim;
}
#endif // FOXTROT_EDITOR
