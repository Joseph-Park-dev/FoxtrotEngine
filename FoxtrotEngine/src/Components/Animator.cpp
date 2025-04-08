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
	SetMeshGroup(ResourceManager::GetInstance()->GetLoadedSpriteAnim(key));
	if (!GetMeshGroup())
		printf("ERROR : Animator::Play()->Animation is null\n");
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
	if (currFrame->Duration <= mAccTime)
	{
		++mCurrFrameIdx;
		if (anim->GetMaxFrameIdx() < mCurrFrameIdx) // if maxIdx is 2, currFrame must be bigger than two
		{
			if (!mIsRepeated)
			{
				mCurrFrameIdx = 0;
				mIsFinished	  = true;
			}
			else
			{
				// Set current frame to the start.
				// (mMaxFrameIdx starts from 0, so the number of frames should be
				// mMaxFrameIdx + 1)
				mCurrFrameIdx -= anim->GetMaxFrameIdx();
			}
		}
		mAccTime = 0.f;
	}
}

void Animator::Initialize(FTCore* coreInstance)
{
	SetRenderer(coreInstance->GetGameRenderer());
	if (0 < mLoadedKeys.size())
		Play(mLoadedKeys.at(0));

	if (0 < MaterialKeys().size())
		GetMeshGroup()->SetMaterials(MaterialKeys(), GetRenderer()->GetDevice());

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
				Play(true);
		}
	}
}

void Animator::UpdatePlayList()
{
	ImGui::Text("Play List");
	UINT key = ChunkKey::NullVal::VALUE_NOT_ASSIGNED;
	FTEditorUtils::DisplayResSelection<FTSpriteAnimation>(
		"Load Animation",
		ResourceManager::GetInstance()->GetSpriteAnimMap(),
		key);

	if (key != ChunkKey::NullVal::VALUE_NOT_ASSIGNED)
	{
		mLoadedKeys.push_back(key);
		if (mLoadedKeys.size() == 1)
			Play(key);
	}

	if (0 < mLoadedKeys.size())
	{
		for (size_t i = 0; i < mLoadedKeys.size(); ++i)
		{
			FTSpriteAnimation* anim = ResourceManager::GetInstance()->GetLoadedSpriteAnim(mLoadedKeys.at(i));
			anim->UpdateUI();
		}
	}
}
#endif // FOXTROT_EDITOR