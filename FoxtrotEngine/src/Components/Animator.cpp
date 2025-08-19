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
	: TileMapRenderer(owner)
	, mLoadedAnim(DBG_NEW FTDS::DynamicArray<FTSpriteAnimation*>)
	, mCurrAnim(nullptr)
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

void Animator::Play(const UINT key, bool isRepeated)
{
	FTSpriteAnimation* anim = mLoadedAnim->At(key);
	mCurrAnim				= anim;
	SetMeshGroup(mCurrAnim);

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
	MeshRenderer::SaveProperties(ofs);

	// Loop through loaded animation keys and save.
	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::Animation::LOADED_KEYS);
	size_t i = 0;
	mLoadedAnim->IterateArray([&](FTSpriteAnimation* anim) {
		FileIOHelper::SaveString(ofs, std::to_string(i).c_str(), anim->FileName());
		++i;
	});
	FileIOHelper::EndDataPackSave(ofs, ChunkKey::Animation::LOADED_KEYS);
}

void Animator::LoadProperties(std::ifstream& ifs)
{
	// Load Animations
	std::pair<size_t, FTDS::String> pack = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::Animation::LOADED_KEYS);
	mLoadedAnim->Reserve(pack.first);
	for (size_t i = 0; i < pack.first; ++i)
	{
		FTDS::String key;
		FileIOHelper::LoadBasicString(ifs, key);

#ifdef FOXTROT_EDITOR
		FTSpriteAnimation* anim = EditorResourceManager::GetInstance()->GetLoadedSpriteAnim(key);
#else
		FTSpriteAnimation* anim = ResourceManager::GetInstance()->GetLoadedSpriteAnim(key);
#endif // FOXTROT_EDITOR

		mLoadedAnim->PushBack(anim);
	}
	mLoadedAnim->Reverse();

	MeshRenderer::LoadProperties(ifs);
}

void Animator::UpdateFrame(float deltaTime)
{
	if (mIsFinished)
		return;
	mAccTime += deltaTime;
	FTSpriteAnimation* anim		 = static_cast<FTSpriteAnimation*>(GetMeshGroup());
	AnimationFrame*	   currFrame = anim->GetFrame(mCurrFrameIdx);
	if (currFrame)
	{
		if (currFrame->Duration <= mAccTime)
		{
			++mCurrFrameIdx;
			mAccTime = 0.f;
		}
	}
	if (IndexOutOfRange(anim->GetMinFrameIdx(), anim->GetMaxFrameIdx()))
	{
		if (!mIsRepeated)
			mIsFinished = true;
		mCurrFrameIdx = 0;
	}
}

bool Animator::IndexOutOfRange(int minIdx, int maxIdx)
{
	return (maxIdx - minIdx) < mCurrFrameIdx;
}

void Animator::Initialize(FTCore* coreInstance)
{
	SetRenderer(coreInstance->GetGameRenderer());

	if (0 < mLoadedAnim->GetSize())
		Play(0);

	Component::Initialize(coreInstance);
}

void Animator::LateUpdate(float deltaTime)
{
	if (!GetMeshGroup())
		return;
	UpdateFrame(deltaTime);
}

void Animator::Render(FoxtrotRenderer* renderer)
{
	if (GetMeshGroup())
	{
		MeshRenderer::UpdateMesh(GetOwner()->GetTransform(), Camera::GetInstance(), renderer);
		renderer->SwitchFillMode();
		// renderer->SetRenderTargetView();
		GetMeshGroup()->Render(mCurrFrameIdx, renderer, GetTexture(), GetVS(), GetPS(), GetMaterial());
	}
}

// void Animator::Render(FoxtrotRenderer* renderer)
//{
//	if (mCurrentAnim != nullptr)
//		mCurrentAnim->Render(renderer, mCurrFrameIdx);
// }

void Animator::CloneTo(Actor* actor)
{
	Animator* newComp = DBG_NEW Animator(actor, GetUpdateOrder());

	newComp->mLoadedAnim->Assign(this->mLoadedAnim);
	newComp->SetRenderer(this->GetRenderer());
	newComp->SetMeshGroup(this->GetMeshGroup());
	newComp->SetTexture(this->GetTexture());
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
	CHECK_RENDERER(GetRenderer());

	UpdatePlayAnim();
	UpdatePlayList();

	CommandHistory::GetInstance()->UpdateBoolValue("Is Repeated", mIsRepeated);
	MeshRenderer::EditorUIUpdate();
}

void Animator::EditorRender(FoxtrotRenderer* renderer)
{
	if (GetMeshGroup())
	{
		MeshRenderer::UpdateMesh(GetOwner()->GetTransform(), EditorCamera::GetInstance(), renderer);
		renderer->SwitchFillMode();
		// renderer->SetRenderTargetView();
		GetMeshGroup()->Render(
			mCurrFrameIdx,
			renderer,
			GetTexture(),
			GetVS(),
			GetPS(),
			GetMaterial());
	}
}

void Animator::UpdatePlayAnim()
{
	if (GetMeshGroup())
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
	ImGui::Text("Play List");
	FTDS::String key = ChunkKey::NullVal::NULL_OBJECT;
	FTEditorUtils::DisplayResSelection<FTSpriteAnimation>(
		"Load Animation",
		ResourceManager::GetInstance()->GetSpriteAnimations(),
		key);

	if (key.NotEqual(ChunkKey::NullVal::NULL_OBJECT))
	{
		FTSpriteAnimation* anim = ResourceManager::GetInstance()->GetLoadedSpriteAnim(key);
		if (mLoadedAnim->GetSize() == 1)
		{
			mCurrAnim = anim;
			SetMeshGroup(mCurrAnim);
		}
		mLoadedAnim->PushBack(anim);
	}

	if (0 < mLoadedAnim->GetSize())
	{
		size_t i = 0;

		mLoadedAnim->IterateArray([&](FTSpriteAnimation* anim) {
			ImGui::PushID(anim->FileName().C_Str());
			ImGui::Text(anim->FileName().C_Str());
			anim->UpdateUI();

			if (ImGui::ArrowButton("##Up", ImGuiDir::ImGuiDir_Up))
				mLoadedAnim->Swap(i - 1, i);
			ImGui::SameLine();
			if (ImGui::ArrowButton("##Down", ImGuiDir::ImGuiDir_Down))
				mLoadedAnim->Swap(i + 1, i);

			if (ImGui::Button("Delete"))
				mLoadedAnim->Erase(i);

			ImGui::PopID();
			++i;
		});
	}
}
#endif // FOXTROT_EDITOR