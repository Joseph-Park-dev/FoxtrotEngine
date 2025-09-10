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
			FTSpriteAnimationDef resDef;

			// Update file name of the sprite animation.
			static FTDS::String name = "Empty Value";
			CommandHistory::GetInstance()->UpdateStringValue("Name", name);

			// Is the sprite animation to be repeated?
			CommandHistory::GetInstance()->UpdateBoolValue("Is repeated", resDef.IsRepeated);

			// Update texture (sprite sheet) of the sprite animation.
			static FTTexture* texture;
			FTEditorUtils::DisplayResSelection("Select Sprite", ResourceManager::GetInstance()->GetTextures(), texture);
			if (texture)
				ImGui::Text(texture->GetFileName().C_Str());
			else
				ImGui::Text("Texture not selected");

			// Update JSON (spritet sheet data) of the sprite animation.
			static FTJSON* JSON;
			FTEditorUtils::DisplayResSelection("Select JSON", ResourceManager::GetInstance()->GetJSONs(), JSON);
			if (JSON)
			{
				ImGui::Text(JSON->GetFileName().C_Str());
				resDef.MinFrameIdx = 0;
				resDef.MaxFrameIdx = static_cast<int>(JSON->Data()[SpriteSheetKeys::BASE].size()) - 1;
			}
			else
				ImGui::Text("JSON not selected");

			// Update FPS of the sprite animation.
			CommandHistory::GetInstance()->UpdateIntValue("FPS", resDef.FPS);

			CommandHistory::GetInstance()->UpdateIntValue(
				"Anim Start Index",
				resDef.MinFrameIdx,
				-resDef.MaxFrameIdx,
				resDef.MaxFrameIdx);

			CommandHistory::GetInstance()->UpdateIntValue("Anim End Index", resDef.MaxFrameIdx, resDef.MinFrameIdx, resDef.MaxFrameIdx);

			// Save sprite animation into a file.
			if (ImGui::Button("Create"))
			{
				if (!name.Contains(FileTypes::SPRITE_ANIMATION))
					name.Append(FileTypes::SPRITE_ANIMATION);
				resDef.FileName = name.C_Str();

				// Update the relative path of the sprite animation.
				FTDS::String path = ResourceManager::GetInstance()->GetPathToAsset();
				path.Append(resDef.FileName);
				resDef.RelativePath = path.C_Str();

				resDef.JSON = JSON;

				FTSpriteAnimation* anim = CreateAnimationFromJSON(resDef);

				// Load the created animation to ResourceManager & File.
				// This is called only during the FTEditor Runtime.
				SaveAnimationAsFile(anim, FileTypes::SPRITE_ANIMATION);
			}
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

FTSpriteAnimation* AnimationManager::CreateAnimationFromJSON(FTSpriteAnimationDef& resDef)
{
	FTSpriteAnimation* anim = DBG_NEW FTSpriteAnimation(resDef, mRenderer);

	SaveAnimationAsFile(anim, FileTypes::SPINE_ANIMATION);
	ResourceManager::GetInstance()->GetSpriteAnimations()->Insert(anim->GetFileName(), anim);
	return anim;
}

FTSpineAnimation* AnimationManager::CreateAnimationFromSpine(const char* name, FTJSON* json, FTText* atlas)
{
	FTResourceDef	  resDef = { name, ResourceManager::GetInstance()->GetPathToAsset().C_Str() };
	FTSpineAnimation* anim	 = DBG_NEW FTSpineAnimation(resDef, mRenderer, json, atlas);

	SaveAnimationAsFile(anim, FileTypes::SPRITE_ANIMATION);
	ResourceManager::GetInstance()->GetSpineAnimations()->Insert(anim->GetFileName(), anim);
	return anim;
}
#endif // FOXTROT_EDITOR
