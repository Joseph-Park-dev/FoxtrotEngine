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

#ifdef FOXTROT_EDITOR
	#include "EditorUtils.h"
	#include "EditorResourceManager.h"
#endif // FOXTROT_EDITOR

Animator::Animator(Actor* owner, int updateOrder)
	: TileMapRenderer(owner)
	, mLoadedKeys()
	, mCurrFrameIdx(0)
	, mAccTime(0.f)
	, mIsFinished(false)
	, mIsRepeated(true)
{
}

Animator::~Animator()
{
	mLoadedKeys.clear();
}

void Animator::Play(const UINT key, bool isRepeated)
{
	UINT loadedKey = mLoadedKeys.at(key);

#ifdef FOXTROT_EDITOR
	SetMeshGroup(EditorResourceManager::GetInstance()->GetLoadedSpriteAnim(loadedKey));
#else
	SetMeshGroup(ResourceManager::GetInstance()->GetLoadedSpriteAnim(loadedKey));
#endif // FOXTROT_EDITOR
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
	Component::SaveProperties(ofs);

	// Loop through material keys and save.
	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::MATERIAL_KEYS);

	for (size_t i = 0; i < MaterialKeys().size(); ++i)
		FileIOHelper::SaveUnsignedInt(ofs, std::to_string(i), MaterialKeys().at(i));
	FileIOHelper::SaveSize(ofs, ChunkKey::MATERIAL_COUNT, MaterialKeys().size());

	FileIOHelper::EndDataPackSave(ofs, ChunkKey::MATERIAL_KEYS);

	// Loop through loaded animation keys and save.
	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::LOADED_KEYS);

	for (size_t i = 0; i < mLoadedKeys.size(); ++i)
		FileIOHelper::SaveUnsignedInt(ofs, std::to_string(i), mLoadedKeys.at(i));

	FileIOHelper::EndDataPackSave(ofs, ChunkKey::LOADED_KEYS);
}

void Animator::LoadProperties(std::ifstream& ifs)
{
	// Load Animations
	std::pair<size_t, std::string> pack = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::LOADED_KEYS);
	mLoadedKeys.reserve(pack.first);
	for (size_t i = 0; i < pack.first; ++i)
	{
		UINT key = 0;
		FileIOHelper::LoadUnsignedInt(ifs, key);
		mLoadedKeys.push_back(key);
	}
	
	std::reverse(mLoadedKeys.begin(), mLoadedKeys.end());

	// Load Materials.
	FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::MATERIAL_KEYS);
	size_t matCount = 0;
	FileIOHelper::LoadSize(ifs, matCount);
	for (size_t i = 0; i < matCount; ++i)
	{
		UINT key = ChunkKey::NullVal::VALUE_NOT_ASSIGNED;
		FileIOHelper::LoadUnsignedInt(ifs, key);
		MaterialKeys().push_back(key);
	}

	Component::LoadProperties(ifs);
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

#ifdef FOXTROT_EDITOR

	std::vector<UINT>::iterator iter = mLoadedKeys.begin();
	auto& animMap = EditorResourceManager::GetInstance()->GetSpriteAnimMap();

	for (; iter != mLoadedKeys.end(); ++iter)
	{
		if (animMap.find(*iter) == animMap.end())
		{
			std::iter_swap(iter, mLoadedKeys.end()-1);
			mLoadedKeys.pop_back();
		}
	}
	//std::sort(mLoadedKeys.begin(), mLoadedKeys.end());

#endif // FOXTROT_EDITOR
	
	if (0 < mLoadedKeys.size())
		Play(0);

	if (GetMeshGroup())
	{
		if (0 < MaterialKeys().size())
			GetMeshGroup()->SetMaterials(MaterialKeys(), GetRenderer()->GetDevice());
	}

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
		UpdateMesh(GetOwner()->GetTransform(), Camera::GetInstance());
		renderer->SwitchFillMode();
		// renderer->SetRenderTargetView();
		GetMeshGroup()->Render(renderer, mCurrFrameIdx);
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
	for (size_t i = 0; i < mLoadedKeys.size(); ++i)
		newComp->mLoadedKeys.push_back(mLoadedKeys.at(i));
	for (size_t i = 0; i < MaterialKeys().size(); ++i)
		newComp->MaterialKeys().push_back(MaterialKeys().at(i));
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
	Render(renderer);
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
				Play(mCurrAnimKey);
		}
	}
}

void Animator::UpdatePlayList()
{
	ImGui::Text("Play List");
	UINT key = ChunkKey::NullVal::VALUE_NOT_ASSIGNED;
	FTEditorUtils::DisplayResSelection<FTSpriteAnimation>(
		"Load Animation",
		EditorResourceManager::GetInstance()->GetSpriteAnimMap(),
		key);

	if (key != ChunkKey::NullVal::VALUE_NOT_ASSIGNED)
	{
		mLoadedKeys.push_back(key);
		if (mLoadedKeys.size() == 1)
		{
			mCurrAnimKey = 0;
			Play(mCurrAnimKey);
		}
	}

	if (0 < mLoadedKeys.size())
	{
		for (size_t i = 0; i < mLoadedKeys.size(); ++i)
		{
			FTSpriteAnimation* anim = EditorResourceManager::GetInstance()->GetLoadedSpriteAnim(mLoadedKeys.at(i));

			ImGui::PushID(anim->GetFileName().c_str());
			ImGui::Text(anim->GetFileName().c_str());
			anim->UpdateUI();

			if (ImGui::ArrowButton("##Up", ImGuiDir::ImGuiDir_Up))
				std::iter_swap(mLoadedKeys.begin() + i - 1, mLoadedKeys.begin() + i);
			ImGui::SameLine();
			if (ImGui::ArrowButton("##Down", ImGuiDir::ImGuiDir_Down))
				std::iter_swap(mLoadedKeys.begin() + i + 1, mLoadedKeys.begin() + i);
			ImGui::PopID();
		}
	}
}
#endif // FOXTROT_EDITOR