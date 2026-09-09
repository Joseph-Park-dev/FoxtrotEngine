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
#include "Manager/DirectoryHelper.h"
#include "FileSystem/NullKeys.h"
#include "Renderer/D3D11Renderer.h"

#include "FTDS/Static/FTString.h"
#include "Manager/ResourceManager.h"

#ifdef FOXTROT_EDITOR
	#include "EditorUtils.h"
	#include "Utility/EditorHelper.h"
	#include "FileSystem/BufferSizes.h"
	#include "FileSystem/DLLPath.h"
// NULLPATH WARNING: This relative path include is problematic.
// It references "GetJSONs"/GetTexts which are defined in Core/include/Manager/ResourceManager.h
// Consider moving Core::ProcName to a separate header to avoid ResourceManager.h name collision.
#endif // FOXTROT_EDITOR

namespace D3D11
{
	using namespace Common;
	using namespace Core;

	/// @brief Returns the spine loader used by this animation manager.
	/// @return Borrowed access to the spine loader.
	FTSpineLoader* AnimationManager::GetSpineLoader()
	{
		return mSpineLoader;
	}

	/// @brief Initializes the services and state required before this object's runtime lifecycle begins.
	/// @param renderer Renderer providing the graphics device and current render state.
	void AnimationManager::Initialize(D3D11::D3D11Renderer* renderer)
	{
		mRenderer = renderer;
#ifdef FOXTROT_EDITOR
		HMODULE coreMod = GetModuleHandleA(Common::DLLPath::CORE_EDITOR);
		FARPROC proc	= GetProcAddress(coreMod, "GetJSONs");
		GetJSONsFunc	= reinterpret_cast<GET_JSON_FUNC>(proc);

		proc		 = GetProcAddress(coreMod, "GetTexts");
		GetTextsFunc = reinterpret_cast<GET_TEXT_FUNC>(proc);

		proc			 = GetProcAddress(coreMod, Core::ProcName::GetAssetPath);
		GetAssetPathFunc = reinterpret_cast<GET_ASSET_PATH_FUNC>(proc);
#endif // FOXTROT_EDITOR
	}

	/// @brief Initializes the registered animation clips.
	/// @note Initializes the :AnimationManager base or delegates to its constructor.
	AnimationManager::AnimationManager()
		: mRenderer(nullptr)
		, mSpineLoader(new FTSpineLoader)
	{
	}

	/// @brief Releases the resources managed by this instance during destruction.
	AnimationManager::~AnimationManager()
	{
		delete mSpineLoader;
	}

#ifdef FOXTROT_EDITOR
	/// @brief Builds the editor controls for inspecting and modifying this object's state.
	/// @param opened Whether the editor window remains open.
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
			Common::ResourcePack<FTSpriteAnimation>* spriteAnims =
				ResourceManager::GetInstance()->GetSpriteAnimations();
			auto iter = spriteAnims->GetResMap()->Begin();

			if (ImGui::TreeNode("Loaded Sprite Animations"))
			{
				spriteAnims->GetResMap()->IterateAllValues([&](FTResource* res) {
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

			Common::ResourcePack<FTSpineAnimation>* spineAnims =
				ResourceManager::GetInstance()->GetSpineAnimations();

			if (ImGui::TreeNode("Loaded Spine Animations"))
			{
				spineAnims->GetResMap()->IterateAllValues([&](FTResource* res) {
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

	/// @brief Displays the controls used to define and create an animation resource.
	void AnimationManager::CreateAnimationGUI()
	{
		ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
		if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
		{
			if (ImGui::BeginTabItem("Sprite Anim"))
			{
				FTSpriteAnimationDef resDef;

				// Update file name of the sprite animation.
				static Common::FTDS::String name = "Empty Value";
				Editor::UPDATE_STR("Name", name);

				// Is the sprite animation to be repeated?
				Editor::UPDATE_BOOL("Is repeated", resDef.IsRepeated);

				// Update texture (sprite sheet) of the sprite animation.
				static FTSprite* texture;
				Editor::DisplayResSelection("Select Sprite", ResourceManager::GetInstance()->GetSprites(), texture);
				if (texture)
					ImGui::Text(texture->GetFileName()->C_Str());
				else
					ImGui::Text("Texture not selected");

				// Update JSON (spritet sheet data) of the sprite animation.
				static GenericData::FTJSON* JSON;
				Editor::DisplayResSelection("Select JSON", GetJSONsFunc(), JSON);
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
					if (!name.Contains(Common::FileTypes::SPRITE_ANIMATION))
						name.Append(Common::FileTypes::SPRITE_ANIMATION);
					resDef.FileName = name.C_Str();

					// Update the relative path of the sprite animation.
					Common::FTDS::String* path = GetAssetPathFunc();
					path->Append(resDef.FileName);
					resDef.Path = path->C_Str();

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
				Common::FTResourceDef resDef;

				static Common::FTDS::String name = "Empty Value";
				Editor::UPDATE_STR("Name", name);

				static GenericData::FTJSON* json;
				Editor::DisplayResSelection("Select Skeleton Data", GetJSONsFunc(), json);

				static GenericData::FTText* atlasTxt;
				Editor::DisplayResSelection("Select Spine Atlas", GetTextsFunc(), atlasTxt);

				if (ImGui::Button("Create"))
				{
					if (!name.Contains(Common::FileTypes::SPINE_ANIMATION))
						name.Append(Common::FileTypes::SPINE_ANIMATION);
					resDef.FileName = name.C_Str();

					// Update the relative path of the sprite animation.
					Common::FTDS::String* path = GetAssetPathFunc();
					path->Append(resDef.FileName);
					resDef.Path = path->C_Str();

					CreateAnimationFromSpine(resDef, json->GetRelativePath(), atlasTxt->GetRelativePath());
				}

				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}
	}

	/// @brief Returns the jsons func getter used by this animation manager.
	/// @return Borrowed access to the jsons func getter.
	/// @note Changes through the returned reference affect this object's stored state.
	GET_JSON_FUNC& AnimationManager::GetJSONsFuncGetter()
	{
		return GetJSONsFunc;
	}

	/// @brief Returns the texts func getter used by this animation manager.
	/// @return Borrowed access to the texts func getter.
	/// @note Changes through the returned reference affect this object's stored state.
	GET_TEXT_FUNC& AnimationManager::GetTextsFuncGetter()
	{
		return GetTextsFunc;
	}

	/// @brief Loads an animation clip from its JSON description and registers it.
	/// @param resDef Resource definition containing the filename and source path.
	/// @return Created animation from json instance or resource.
	FTSpriteAnimation* AnimationManager::CreateAnimationFromJSON(FTSpriteAnimationDef& resDef)
	{
		FTSpriteAnimation* anim = DBG_NEW FTSpriteAnimation(resDef, mRenderer);

		SaveAnimationAsFile(anim);
		D3D11::ResourceManager::GetInstance()->GetSpriteAnimations()->AddResource(anim->GetFileName(), anim);
		return anim;
	}

	/// @brief Builds an animation resource from Spine skeleton and atlas data.
	/// @param resDef Resource definition containing the filename and source path.
	/// @param jsonPath Path to the JSON resource description.
	/// @param atlasPath Path to the Spine atlas file.
	/// @return Created animation from spine instance or resource.
	FTSpineAnimation* AnimationManager::CreateAnimationFromSpine(Common::FTResourceDef& resDef, const Common::FTDS::String* jsonPath, const Common::FTDS::String* atlasPath)
	{
		FTSpineAnimation* anim = DBG_NEW FTSpineAnimation(resDef, mRenderer, jsonPath, atlasPath);
		SaveAnimationAsFile(anim);
		D3D11::ResourceManager::GetInstance()->GetSpineAnimations()->AddResource(anim->GetFileName(), anim);
		return anim;
	}
#endif // FOXTROT_EDITOR
} // namespace D3D11
