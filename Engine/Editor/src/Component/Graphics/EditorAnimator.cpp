// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Component/Graphics/EditorAnimator.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>

#include "EditorUtils.h"
#include "EditorCamera.h"
#include "Utility/EditorHelper.h"
#include "CommandHistory.h"
#include "EditorRenderer.h"

#include "ResourceSystem/Animation/FTSpriteAnimation.h"
#include "Manager/AnimationManager.h"
#include <../D3D11/include/Manager/ResourceManager.h>

namespace Editor
{
	constexpr const char* GET_SPRITE_ANIMS = "GetSpriteAnimations";
	using namespace Core;
	using namespace D3D11;
	void EditorAnimator::EditorUpdate(float deltaTime)
	{
		LateUpdate(deltaTime);
	}

	void EditorAnimator::EditorUIUpdate()
	{
		UpdatePlayAnim();
		UpdatePlayList();

		CommandHistory::GetInstance()->UpdateBoolValue("Is Repeated", IsRepeated());
		SpriteRenderer::EditorUIUpdate();
	}

	EditorAnimator::EditorAnimator(Core::IActor* owner, int updateOrder)
		: Animator(owner, updateOrder)
	{
		// mGetSpriteAnimFunc = GetFunc<GET_SPRITE_ANIMS_FUNC>(DLLPaths::D3D11_EDITOR, GET_SPRITE_ANIMS);
	}

	void EditorAnimator::EditorRender(Core::IRenderer* renderer, Core::ICamera* camInst)
	{
		if (GetSprite())
		{
			// renderer->SetFillMode();

			Transform*		   transform = GetOwner()->GetTransform();
			FTSpriteAnimation* anim		 = static_cast<FTSpriteAnimation*>(GetSprite());
			EditorRenderer*	   rend		 = reinterpret_cast<EditorRenderer*>(renderer);

			GetSprite()->UpdateConstantBuffers(
				renderer,
				transform,
				camInst,
				GetMaterial(),
				anim->GetFrameCount(),
				CurrFrameIdx());

			GetSprite()->Render(
				renderer,
				transform,
				camInst,
				GetPSO(),
				GetMaterial());
		}
	}

	void EditorAnimator::UpdatePlayAnim()
	{
		if (GetSprite())
		{
			if (IsFinished())
			{
				if (ImGui::Button("Stop"))
					Stop();
			}
			else
			{
				if (ImGui::Button("Play"))
					Play(0);
			}
		}
	}

	void EditorAnimator::UpdatePlayList()
	{
		FTSpriteAnimation* anim = nullptr;
		Editor::DisplayResSelection<FTSpriteAnimation>(
			"Load Animation",
			D3D11::ResourceManager::GetInstance()->GetSpriteAnimations(),
			anim);

		if (anim)
		{
			LoadedAnim()->PushBack(anim);
			if (LoadedAnim()->GetSize() == 1)
				SetSprite(anim);
		}

		ImGui::SeparatorText("Play List");
		if (0 < LoadedAnim()->GetSize())
		{
			size_t i = 0;

			LoadedAnim()->IterateArray([&](FTSpriteAnimation* anim) {
				if (anim)
				{
					ImGui::PushID(anim);
					ImGui::Text(anim->GetFileName()->C_Str());
					anim->UpdateUI();

					if (ImGui::ArrowButton("##Up", ImGuiDir::ImGuiDir_Up))
						LoadedAnim()->Swap(i - 1, i);
					ImGui::SameLine();
					if (ImGui::ArrowButton("##Down", ImGuiDir::ImGuiDir_Down))
						LoadedAnim()->Swap(i + 1, i);

					if (ImGui::Button("Update"))
						D3D11::AnimationManager::GetInstance()->SaveAnimationAsFile(anim);

					if (ImGui::Button("Delete"))
					{
						anim->SubtractRefCount();
						LoadedAnim()->Erase(i);
						ImGui::PopID();
						return;
					}

					ImGui::PopID();
					++i;
				}
			});
		}
		ImGui::Separator();
	}
} // namespace Editor