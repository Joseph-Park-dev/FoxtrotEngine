#include "AnimationManager.h"

#include <Windows.h>
#include "spine/spine.h"

#include "ResourceSystem/Animation/FTSpriteAnimation.h"
#include "ResourceSystem/Animation/FTSpineAnimation.h"
#include "ResourceSystem/FTTileMap.h"
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
		CreateAnimationGUI();

		ImGui::Separator();
		FTDS::HashMap<FTSpriteAnimation*>* map =
			ResourceManager::GetInstance()->GetSpriteAnimations();
		auto iter = map->Begin();

		if (ImGui::TreeNode("Loaded Sprite Animations"))
		{
			map->IterateAllValues([&](FTSpriteAnimation* anim) {
				if (anim)
				{
					ImGui::PushID(anim->GetFileName().C_Str());
					if (ImGui::BeginListBox(anim->GetFileName().C_Str(), ImVec2(-FLT_MIN, 100)))
					{
						ImGui::Text(anim->GetFileName().C_Str());
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
			ResourceManager::GetInstance()->GetSpineAnimations();

		if (ImGui::TreeNode("Loaded Spine Animations"))
		{
			spineAnimMap->IterateAllValues([&](FTSpineAnimation* anim) {
				if (anim)
				{
					ImGui::PushID(anim);
					if (ImGui::BeginListBox(anim->GetFileName().C_Str(), ImVec2(-FLT_MIN, 100)))
					{
						ImGui::Text(anim->GetFileName().C_Str());
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

void AnimationManager::CreateAnimationGUI()
{
	ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
	if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
	{
		if (ImGui::BeginTabItem("Sprite Anim"))
		{
			//static int e = 0;
			//ImGui::RadioButton("JSON", &e, 0);
			//ImGui::SameLine();
			//ImGui::RadioButton("Atlas", &e, 1);

			//if (e == 0)
			//{
			//	static char name[BufferSize::STRING_BUFFER_SIZE] = "Empty Value";
			//	ImGui::InputText("Name", name, BufferSize::STRING_BUFFER_SIZE);

			//	static FTTexture* texture = nullptr;
			//	FTEditorUtils::DisplayResSelection("Select Sprite", ResourceManager::GetInstance()->GetTextures(), texture);
			//	if (texture)
			//		ImGui::Text(texture->GetFileName().C_Str());
			//	else
			//		ImGui::Text("Texture not selected");

			//	static FTJSONSheet* sheet = nullptr;
			//	FTEditorUtils::DisplayResSelection<FTJSONSheet>("Select JSONSheet", ResourceManager::GetInstance()->GetJSONSheets(), sheet);
			//	int maxIdx = 0;
			//	if (sheet)
			//		maxIdx = static_cast<int>(sheet->GetTilesCount()) - 1;

			//	static int startIdx;
			//	static int endIdx;

			//	CommandHistory::GetInstance()->UpdateIntValue("Anim Start Index", startIdx, -maxIdx, maxIdx);
			//	CommandHistory::GetInstance()->UpdateIntValue("Anim End Index", endIdx, -maxIdx, maxIdx);
			//	if (ImGui::Button("Auto Detect"))
			//	{
			//		startIdx = 0;
			//		endIdx	 = maxIdx;
			//	}

			//	if (ImGui::Button("Create"))
			//	{
			//		FTSpriteAnimation* anim = CreateAnimationFromSpriteSheet(name, sheet, startIdx, endIdx);

			//		// Load the created animation to ResourceManager & File.
			//		// This is called only during the FTEditor Runtime.
			//		SaveAnimationAsFile(anim, FileTypes::SPRITE_ANIMATION);
			//	}
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Spine Anim"))
		{
			static char name[BufferSize::STRING_BUFFER_SIZE] = "Empty Value";
			ImGui::InputText("Name", name, BufferSize::STRING_BUFFER_SIZE);

			static FTJSON* json;
			FTEditorUtils::DisplayResSelection("Select Skeleton Data", ResourceManager::GetInstance()->GetJSONs(), json);

			static FTText* atlasTxt;
			FTEditorUtils::DisplayResSelection("Select Spine Atlas", ResourceManager::GetInstance()->GetTexts(), atlasTxt);

			if (ImGui::Button("Create"))
				CreateAnimationFromSpine(name, json, atlasTxt);

			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
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
	// FTEditorUtils::DisplayResSelection("Select SpriteSheet", EditorResourceManager::GetInstance()->GetSpriteSheets(), key);
}

FTSpriteAnimation* AnimationManager::CreateAnimationFromSpineAtlas(const char* name, FTText* sheet, int minIdx, int maxIdx)
{
	/*if (!mRenderer)
		printf("ERROR : Animator::CreateAnimationFromTile()-> Renderer is null");

	FTSpriteAnimation* animation = DBG_NEW FTSpriteAnimation;

	FTDS::String animName = FTDS::String(name) + FileTypes::SPRITE_ANIMATION;
	animation->SetFileName(animName);
	animation->SetMinFrameIdx(minIdx);
	animation->SetMaxFrameIdx(maxIdx);

	FTDS::String path = ResourceManager::GetInstance()->GetPathToAsset();
	path.Append(animName);
	animation->SetRelativePath(path);
	animation->Initialize(sheet, mRenderer->GetDevice(), mRenderer->GetContext());

	ResourceManager::GetInstance()->GetSpriteAnimations()->Insert(animation->FileName(), animation);
	printf("FTSpriteAnimation created, %s\n", name);

	return animation;*/
	return nullptr;
}

FTSpineAnimation* AnimationManager::CreateAnimationFromSpine(const char* name, FTJSON* json, FTText* atlas)
{
	FTResourceDef	  resDef = { name, ResourceManager::GetInstance()->GetPathToAsset().C_Str() };
	FTSpineAnimation* anim = DBG_NEW FTSpineAnimation(resDef, mRenderer, json, atlas);

	SaveAnimationAsFile(anim, FileTypes::SPINE_ANIMATION);
	ResourceManager::GetInstance()->GetSpineAnimations()->Insert(anim->GetFileName(), anim);
	return anim;
}
#endif // FOXTROT_EDITOR
