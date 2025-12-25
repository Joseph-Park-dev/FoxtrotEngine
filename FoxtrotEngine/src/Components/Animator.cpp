// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Components/Animator.h"

#include "Actors/Actor.h"
#include "Actors/Transform.h"
#include "ResourceSystem/Tile.h"
#include "ResourceSystem/FTTexture.h"
#include "ResourceSystem/Animation/FTSpriteAnimation.h"
#include "ResourceSystem/Animation/AnimationFrame.h"
#include "ResourceSystem/GeometryGenerator.h"
#include "Managers/ResourceManager.h"
#include "Managers/AnimationManager.h"
#include "Core/TemplateFunctions.h"
#include "Core/FTCore.h"
#include "FileSystem/ChunkLoader.h"
#include "FileSystem/FileIOHelper.h"
#include "FileSystem/BufferSizes.h"
#include "Renderer/FoxtrotRenderer.h"
#include "Renderer/Camera.h"

#include "ResourceSystem/FTShaders/FTVertexShader.h"
#include "ResourceSystem/FTShaders/FTPixelShader.h"

#ifdef FOXTROT_EDITOR
	#include "EditorUtils.h"
	#include "EditorResourceManager.h"
	#include "EditorCamera.h"
	#include "ResourceSystem/FTMaterials/FTMaterial.h"
#endif // FOXTROT_EDITOR

Animator::Animator(Actor* owner, int updateOrder)
	: SpriteRenderer(owner)
	, mLoadedAnim(DBG_NEW FTDS::DynamicArray<FTSpriteAnimation*>)
	, mCurrFrameIdx(0)
	, mAccTime(0.f)
	, mIsFinished(false)
	, mIsRepeated(true)
{
}

Animator::~Animator()
{
	mLoadedAnim->Clear();
	delete mLoadedAnim;
}

void Animator::Play(const UINT idx, bool isRepeated)
{
	FTSpriteAnimation* anim = mLoadedAnim->At(idx);
	SetSprite(anim);

	mIsFinished = false;
	mIsRepeated = isRepeated;
}

void Animator::Stop()
{
	mIsFinished = true;
}

bool Animator::GetIsFinished() const { return mIsFinished; }
int	 Animator::GetCurrFrameIdx() const { return mCurrFrameIdx; }

void Animator::SetFrame(int frameNumber)
{
	mIsFinished	  = false;
	mCurrFrameIdx = frameNumber;
	mAccTime	  = 0.f;
}
void Animator::SetIsFinished(bool val) { mIsFinished = val; }

void Animator::SaveProperties(std::ofstream& ofs)
{
	SpriteRenderer::SaveProperties(ofs);

	// Loop through loaded animation keys and save.
	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::FTSpriteAnimator::LOADED_KEYS);
	size_t i = 0;
	mLoadedAnim->IterateArray([&](FTSpriteAnimation* anim) {
		if (anim)
			FileIOHelper::SaveString(ofs, std::to_string(i).c_str(), anim->GetFileName());
		++i;
	});
	FileIOHelper::EndDataPackSave(ofs, ChunkKey::FTSpriteAnimator::LOADED_KEYS);
}

void Animator::LoadProperties(std::ifstream& ifs)
{
	// Load Animations
	std::pair<size_t, FTDS::String> pack = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FTSpriteAnimator::LOADED_KEYS);
	mLoadedAnim->Reserve(pack.first);
	for (size_t i = 0; i < pack.first; ++i)
	{
		FTDS::String key;
		FileIOHelper::LoadBasicString(ifs, key);

		FTSpriteAnimation* anim = ResourceManager::GetInstance()->GetLoadedSpriteAnim(key);
		mLoadedAnim->PushBack(anim);
	}

	mLoadedAnim->Reverse();
	SpriteRenderer::LoadProperties(ifs);

	if (0 < mLoadedAnim->GetSize())
		Play(0);
}

void Animator::UpdateFrame(float deltaTime)
{
	if (mIsFinished)
		return;
	mAccTime += deltaTime;
	FTSpriteAnimation* anim				= static_cast<FTSpriteAnimation*>(GetSprite());
	static float	   durationPerFrame = 1 / static_cast<float>(anim->GetFPS());

	if (durationPerFrame <= mAccTime)
	{
		++mCurrFrameIdx;
		mAccTime = 0.f;
	}
	if (IndexOutOfRange(anim))
	{
		if (!mIsRepeated)
			mIsFinished = true;
		mCurrFrameIdx = 0;
	}
}

bool Animator::IndexOutOfRange(FTSpriteAnimation* anim)
{
	return (anim->GetMaxFrameIdx() - anim->GetMinFrameIdx()) < mCurrFrameIdx;
}

void Animator::Initialize(FTCore* coreInstance)
{
	if (0 < mLoadedAnim->GetSize())
		Play(0);

	Component::Initialize(coreInstance);
}

void Animator::LateUpdate(float deltaTime)
{
	if (!GetSprite())
		return;
	UpdateFrame(deltaTime);
}

void Animator::Render(FoxtrotRenderer* renderer)
{
	if (GetSprite())
	{
		renderer->SwitchFillMode();

		Transform* transform = GetOwner()->GetTransform();
		static_cast<FTSpriteAnimation*>(GetSprite())
			->Render(
				mCurrFrameIdx,
				renderer,
				transform,
				Camera::GetInstance(),
				GetVS(),
				GetGS(),
				GetPS(),
				GetMaterial());
	}
}

void Animator::CloneTo(Actor* actor)
{
	Animator* newComp = DBG_NEW Animator(actor, GetUpdateOrder());

	newComp->mLoadedAnim->Assign(this->mLoadedAnim);
	newComp->SetSprite(this->GetSprite());
	newComp->SetVS(this->GetVS());
	newComp->SetPS(this->GetPS());
	newComp->SetMaterial(this->GetMaterial());
}

#ifdef FOXTROT_EDITOR
void Animator::EditorUpdate(float deltaTime)
{
	LateUpdate(deltaTime);
}

void Animator::EditorUIUpdate()
{
	UpdatePlayAnim();
	UpdatePlayList();

	CommandHistory::GetInstance()->UpdateBoolValue("Is Repeated", mIsRepeated);
	SpriteRenderer::EditorUIUpdate();
}

void Animator::EditorRender(FoxtrotRenderer* renderer)
{
	if (GetSprite())
	{
		renderer->SwitchFillMode();
		Transform* transform = GetOwner()->GetTransform();

		static_cast<FTSpriteAnimation*>(GetSprite())
			->Render(
				mCurrFrameIdx,
				renderer,
				transform,
				EditorCamera::GetInstance(),
				GetVS(),
				GetGS(),
				GetPS(),
				GetMaterial());
	}
}

void Animator::UpdatePlayAnim()
{
	if (GetSprite())
	{
		if (mIsFinished)
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

void Animator::UpdatePlayList()
{
	FTSpriteAnimation* anim = nullptr;
	FTEditorUtils::DisplayResSelection<FTSpriteAnimation>(
		"Load Animation",
		ResourceManager::GetInstance()->GetSpriteAnimations(),
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
				ImGui::Text(anim->GetFileName().C_Str());
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
#endif // FOXTROT_EDITOR