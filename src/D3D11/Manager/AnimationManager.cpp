#include "Manager/AnimationManager.h"

#include <Windows.h>
#include <spine/spine.h>

#include "ResourceSystem/Animation/FTSpriteAnimation.h"
#include "ResourceSystem/Animation/FTSpineAnimation.h"
#include "ResourceSystem/Sprite/FTTileMap.h"
#include "ResourceSystem/Mesh/GeometryGenerator.h"
#include "ResourceSystem/GenericData/FTText.h"
#include "ResourceSystem/GenericData/FTJSON.h"
#include "Manager/ResourceManager.h"
#include "Manager/FTSpineLoader.h"
#include "FTCore.h"
#include "FileSystem/NullKeys.h"
#include "Renderer/FoxtrotRenderer.h"

#include "Static/FTString.h"

#ifdef FOXTROT_EDITOR
	#include "EditorUtils.h"
	#include "EditorHelper.h"
	#include "FileSystem/BufferSizes.h"
#endif // FOXTROT_EDITOR

namespace D3D11
{
	using namespace Core;

	FTSpineLoader* AnimationManager::GetSpineLoader()
	{
		return mSpineLoader;
	}

	void AnimationManager::Initialize(D3D11::D3D11Renderer* renderer)
	{
		mRenderer = renderer;
	}

	AnimationManager::AnimationManager()
		: mRenderer(nullptr)
		, mSpineLoader(new FTSpineLoader)
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
			FTDS::HashMap<FTResource*>& map =
				ResourceManager::GetInstance()->GetResMap<FTSpriteAnimation>();
			auto iter = map.Begin();

			if (ImGui::TreeNode("Loaded Sprite Animations"))
			{
				map.IterateAllValues([&](FTResource* res) {
					FTSpriteAnimation* anim = reinterpret_cast<FTSpriteAnimation*>(res);
					if (anim)
					{
						ImGui::PushID(anim->GetFileName()->C_Str());
						if (ImGui::BeginListBox(anim->GetFileName()->C_Str(), ImVec2(-FLT_MIN, 100)))
						{
							ImGui::Text(anim->GetFileName()->C_Str());
							anim->UpdateUI();
							if (ImGui::Button("Save"))
								SaveAnimationAsFile(anim);
							ImGui::EndListBox();
						}
						ImGui::PopID();
					}
				});
				ImGui::TreePop();
			}

			FTDS::HashMap<FTResource*>& spineAnimMap =
				ResourceManager::GetInstance()->GetResMap<FTSpineAnimation>();

			if (ImGui::TreeNode("Loaded Spine Animations"))
			{
				spineAnimMap.IterateAllValues([&](FTResource* res) {
					FTSpineAnimation* anim = reinterpret_cast<FTSpineAnimation*>(res);
					if (anim)
					{
						ImGui::PushID(anim);
						if (ImGui::BeginListBox(anim->GetFileName()->C_Str(), ImVec2(-FLT_MIN, 100)))
						{
							ImGui::Text(anim->GetFileName()->C_Str());
							anim->UpdateUI();
							if (ImGui::Button("Save"))
								SaveAnimationAsFile(anim);
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
				Editor::UPDATE_STR("Name", name);

				// Is the sprite animation to be repeated?
				Editor::UPDATE_BOOL("Is repeated", resDef.IsRepeated);

				// Update texture (sprite sheet) of the sprite animation.
				static FTSprite* texture;
				Editor::DisplayResSelection("Select Sprite", &ResourceManager::GetInstance()->GetResMap<FTSprite>(), texture);
				if (texture)
					ImGui::Text(texture->GetFileName()->C_Str());
				else
					ImGui::Text("Texture not selected");

				// Update JSON (spritet sheet data) of the sprite animation.
				static FTJSON* JSON;
				Editor::DisplayResSelection("Select JSON", &ResourceManager::GetInstance()->GetResMap<FTJSON>(), JSON);
				if (JSON)
				{
					ImGui::Text(JSON->GetFileName()->C_Str());
					resDef.MinFrameIdx = 0;
					resDef.MaxFrameIdx = static_cast<int>(JSON->Data()[SpriteSheetKeys::BASE].size()) - 1;
				}
				else
					ImGui::Text("JSON not selected");

				// Update FPS of the sprite animation.
				Editor::UPDATE_INT("FPS", resDef.FPS);

				Editor::UPDATE_CLAMPED_INT(
					"Anim Start Index",
					resDef.MinFrameIdx,
					-resDef.MaxFrameIdx,
					resDef.MaxFrameIdx);

				Editor::UPDATE_CLAMPED_INT("Anim End Index", resDef.MaxFrameIdx, resDef.MinFrameIdx, resDef.MaxFrameIdx);

				// Save sprite animation into a file.
				if (ImGui::Button("Create"))
				{
					if (!name.Contains(FileTypes::SPRITE_ANIMATION))
						name.Append(FileTypes::SPRITE_ANIMATION);
					resDef.FileName = name.C_Str();

					// Update the relative path of the sprite animation.
					FTDS::String path = DirectoryHelper::GetInstance()->GetAssetPath();
					path.Append(resDef.FileName);
					resDef.Path = path.C_Str();

					resDef.JSON		   = JSON;
					resDef.SpriteSheet = texture->GetTexture();

					FTSpriteAnimation* anim = CreateAnimationFromJSON(resDef);

					// Load the created animation to ResourceManager & File.
					// This is called only during the FTEditor Runtime.
					SaveAnimationAsFile(anim);
				}
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Spine Anim"))
			{
				FTResourceDef resDef;

				static FTDS::String name = "Empty Value";
				Editor::UPDATE_STR("Name", name);

				static FTJSON* json;
				Editor::DisplayResSelection("Select Skeleton Data", &ResourceManager::GetInstance()->GetResMap<FTJSON>(), json);

				static FTText* atlasTxt;
				Editor::DisplayResSelection("Select Spine Atlas", &ResourceManager::GetInstance()->GetResMap<FTText>(), atlasTxt);

				if (ImGui::Button("Create"))
				{
					if (!name.Contains(FileTypes::SPINE_ANIMATION))
						name.Append(FileTypes::SPINE_ANIMATION);
					resDef.FileName = name.C_Str();

					// Update the relative path of the sprite animation.
					FTDS::String path = DirectoryHelper::GetInstance()->GetAssetPath();
					path.Append(resDef.FileName);
					resDef.Path = path.C_Str();

					CreateAnimationFromSpine(resDef, json->GetRelativePath(), atlasTxt->GetRelativePath());
				}

				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}
	}

	FTSpriteAnimation* AnimationManager::CreateAnimationFromJSON(FTSpriteAnimationDef& resDef)
	{
		FTSpriteAnimation* anim = DBG_NEW FTSpriteAnimation(resDef, mRenderer);

		SaveAnimationAsFile(anim);
		D3D11::ResourceManager::GetInstance()->GetResMap<FTSpriteAnimation>().Insert(*anim->GetFileName(), anim);
		return anim;
	}

	FTSpineAnimation* AnimationManager::CreateAnimationFromSpine(FTResourceDef& resDef, const FTDS::String* jsonPath, const FTDS::String* atlasPath)
	{
		FTSpineAnimation* anim = DBG_NEW FTSpineAnimation(resDef, mRenderer, jsonPath, atlasPath);
		SaveAnimationAsFile(anim);
		D3D11::ResourceManager::GetInstance()->GetResMap<FTSpineAnimation>().Insert(*anim->GetFileName(), anim);
		return anim;
	}
#endif // FOXTROT_EDITOR
} // namespace D3D11