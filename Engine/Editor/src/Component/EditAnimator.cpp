// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "EditAnimator.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "EditorUtils.h"
#include "EditorCamera.h"
#include "EditorHelper.h"
#include "CommandHistory.h"

#include "ResourceSystem/Animation/FTSpriteAnimation.h"

namespace Editor
{
	using namespace Core;
	using namespace D3D11;
	void EditAnimator::EditorUpdate(float deltaTime)
	{
		LateUpdate(deltaTime);
	}

	void EditAnimator::EditorUIUpdateImpl()
	{
		UpdatePlayAnim();
		UpdatePlayList();

		Editor::CommandHistory::GetInstance()->UpdateBoolValue("Is Repeated", IsRepeated());
		SpriteRenderer::EditorUIUpdate();
	}

	void EditAnimator::EditorRenderImpl(D3D11::D3D11Renderer* renderer)
	{
	}

	void EditAnimator::EditorUIUpdate()
	{
	}

	void EditAnimator::EditorRender(D3D11::D3D11Renderer* renderer)
	{
		if (GetSprite())
		{
			// renderer->SetFillMode();

			Transform*		   transform = GetOwner()->GetTransform();
			FTSpriteAnimation* anim		 = static_cast<FTSpriteAnimation*>(GetSprite());
			GetSprite()->UpdateConstantBuffers(
				renderer,
				transform,
				Editor::EditorCamera::GetInstance(),
				GetMaterial(),
				anim->GetFrameCount(),
				GetCurrentFrameIdx());

			GetSprite()->Render(
				renderer,
				transform,
				Editor::EditorCamera::GetInstance(),
				GetPSO(),
				GetMaterial());
		}
	}

	void EditAnimator::UpdatePlayAnim()
	{
		if (GetSprite())
		{
			if (GetIsFinished())
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

	void EditAnimator::UpdatePlayList()
	{
		FTSpriteAnimation* anim = nullptr;
		Editor::DisplayResSelection<FTSpriteAnimation>(
			"Load Animation",
			&ResourceManager::GetInstance()->GetResMap<FTSpriteAnimation>(),
			anim);

		if (anim)
		{
			mLoadedAnim->PushBack(anim);
			if (mLoadedAnim->GetSize() == 1)
				SetSprite(anim);
		}

		ImGui::SeparatorText("Play List");
		if (0 < mLoadedAnim->GetSize())
		{
			size_t i = 0;

			mLoadedAnim->IterateArray([&](FTSpriteAnimation* anim) {
				if (anim)
				{
					ImGui::PushID(anim);
					ImGui::Text(anim->GetFileName()->C_Str());
					anim->UpdateUI();

					if (ImGui::ArrowButton("##Up", ImGuiDir::ImGuiDir_Up))
						mLoadedAnim->Swap(i - 1, i);
					ImGui::SameLine();
					if (ImGui::ArrowButton("##Down", ImGuiDir::ImGuiDir_Down))
						mLoadedAnim->Swap(i + 1, i);

					if (ImGui::Button("Update"))
						AnimationManager::GetInstance()->SaveAnimationAsFile(anim);

					if (ImGui::Button("Delete"))
					{
						anim->SubtractRefCount();
						mLoadedAnim->Erase(i);
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