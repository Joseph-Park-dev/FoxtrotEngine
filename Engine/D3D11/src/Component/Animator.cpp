// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Component/Animator.h"

#include "Actor/IActor.h"
#include "Actor/Transform.h"
#include "ResourceSystem/Sprite/Tile.h"
#include "ResourceSystem/FTTexture.h"
#include "ResourceSystem/Animation/FTSpriteAnimation.h"
#include "ResourceSystem/Animation/AnimationFrame.h"
#include "ResourceSystem/Mesh/GeometryGenerator.h"
#include "Manager/ResourceManager.h"
#include "Manager/AnimationManager.h"
#include "TemplateFunctions.h"
#include "FileSystem/ChunkLoader.h"
#include "FileSystem/FileIOHelper.h"
#include "FileSystem/BufferSizes.h"
#include "Renderer/D3D11Renderer.h"
#include "Renderer/Camera.h"

#include "ResourceSystem/Shader/FTVertexShader.h"
#include "ResourceSystem/Shader/FTPixelShader.h"
#include "ResourceSystem/Material/FTMaterial.h"

namespace D3D11
{
	using namespace Common;
	using namespace Core;

	Animator::Animator(Core::IActor* owner, int updateOrder)
		: SpriteRenderer(owner, updateOrder)
		, mLoadedAnim(DBG_NEW Common::FTDS::DynamicArray<D3D11::FTSpriteAnimation*>)
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

	void Animator::Play(const size_t idx, bool isRepeated)
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
		std::pair<size_t, Common::FTDS::String> pack = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FTSpriteAnimator::LOADED_KEYS);
		mLoadedAnim->Reserve(pack.first);
		for (size_t i = 0; i < pack.first; ++i)
		{
			Common::FTDS::String key;
			FileIOHelper::LoadBasicString(ifs, key);

			FTSpriteAnimation* anim = D3D11::ResourceManager::GetInstance()->GetSpriteAnimation(key);
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

	void Animator::Initialize()
	{
		if (0 < mLoadedAnim->GetSize())
			Play(0);

		D3D11Component::Initialize();
	}

	void Animator::ProcessInput(IInputDevice* inputDevice)
	{
	}

	void Animator::Update(float deltaTime)
	{
	}

	void Animator::LateUpdate(float deltaTime)
	{
		if (!GetSprite())
			return;
		UpdateFrame(deltaTime);
	}

	void Animator::Render(D3D11::D3D11Renderer* renderer, D3D11::Camera* camInst)
	{
		if (GetSprite())
		{
			Core::Transform*   transform = GetOwner()->GetTransform();
			FTSpriteAnimation* anim		 = static_cast<FTSpriteAnimation*>(GetSprite());
			GetSprite()->UpdateConstantBuffers(
				renderer,
				transform,
				camInst,
				GetMaterial(),
				anim->GetFrameCount(),
				mCurrFrameIdx);

			GetSprite()->Render(
				renderer,
				transform,
				camInst,
				GetPSO(),
				GetMaterial());
		}
	}

	void Animator::CloneTo(Core::IActor* actor)
	{
		Animator* newComp = DBG_NEW Animator(actor, GetUpdateOrder());

		newComp->mLoadedAnim->Assign(this->mLoadedAnim);
		newComp->SetSprite(this->GetSprite());
		newComp->SetPSO(this->GetPSO());
		newComp->SetMaterial(this->GetMaterial());
	}
} // namespace D3D11