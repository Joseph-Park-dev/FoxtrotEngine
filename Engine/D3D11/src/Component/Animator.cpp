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

#ifdef FOXTROT_EDITOR
	#include "EditorUtils.h"
#endif // FOXTROT_EDITOR
	

namespace D3D11
{
	using namespace Common;
	using namespace Core;

	/// @brief Initializes sprite-animation playback state for its owner.
	/// @param owner Actor or object associated with the new instance.
	/// @param updateOrder Order used when dispatching component updates.
	/// @note Initializes the :Animator base or delegates to its constructor.
	Animator::Animator(Core::IActor* owner, int updateOrder)
		: SpriteRenderer(owner, updateOrder)
		, mLoadedAnim(DBG_NEW Common::FTDS::DynamicArray<D3D11::FTSpriteAnimation*>)
		, mCurrFrameIdx(0)
		, mAccTime(0.f)
		, mIsFinished(false)
		, mIsRepeated(true)
	{
	}

	/// @brief Releases the resources managed by this instance during destruction.
	Animator::~Animator()
	{
		mLoadedAnim->Clear();
		delete mLoadedAnim;
	}

	/// @brief Starts or selects animation playback.
	/// @param idx Zero-based element index.
	/// @param isRepeated Whether playback repeats after its final frame.
	void Animator::Play(const size_t idx, bool isRepeated)
	{
		FTSpriteAnimation* anim = mLoadedAnim->At(idx);
		SetSprite(anim);

		mIsFinished = false;
		mIsRepeated = isRepeated;
	}

	/// @brief Stops animation playback.
	void Animator::Stop()
	{
		mIsFinished = true;
	}

	/// @brief Returns the is finished used by this animator.
	/// @return Current value of the is finished flag.
	bool Animator::GetIsFinished() const { return mIsFinished; }
	/// @brief Returns the curr frame idx used by this animator.
	/// @return Current curr frame idx.
	int	 Animator::GetCurrFrameIdx() const { return mCurrFrameIdx; }

	/// @brief Updates the frame used by subsequent operations.
	/// @param frameNumber Replacement frame.
	void Animator::SetFrame(int frameNumber)
	{
		mIsFinished	  = false;
		mCurrFrameIdx = frameNumber;
		mAccTime	  = 0.f;
	}
	/// @brief Updates the is finished used by subsequent operations.
	/// @param val Replacement is finished.
	void Animator::SetIsFinished(bool val) { mIsFinished = val; }

	/// @brief Serializes this object's persistent properties to a .chunk stream.
	/// @param ofs Output stream receiving the serialized data.
	/// @note Writes to the supplied stream at its current position.
	void Animator::SaveProperties(std::ofstream& ofs)
	{
		SpriteRenderer::SaveProperties(ofs);

		// Loop through loaded animation keys and save.
		FileIOHelper::BeginDataPackSave(ofs, ChunkKey::Animator::LOADED_KEYS);
		size_t i = 0;
		mLoadedAnim->IterateArray([&](FTSpriteAnimation* anim) {
			if (anim)
				FileIOHelper::SaveString(ofs, std::to_string(i).c_str(), anim->GetFileName());
			++i;
		});
		FileIOHelper::EndDataPackSave(ofs, ChunkKey::Animator::LOADED_KEYS);
	}

	/// @brief Restores this object's persistent properties from a .chunk stream.
	/// @param ifs Input stream positioned at the expected data; reading advances its position.
	/// @note Advances the stream position and updates the destination state.
	void Animator::LoadProperties(std::ifstream& ifs)
	{
		// Load Animations
		std::pair<size_t, Common::FTDS::String> pack = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::Animator::LOADED_KEYS);
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

	/// @brief Advances the animation's current frame according to playback state.
	/// @param deltaTime Elapsed frame time in seconds.
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

	/// @brief Tests whether an index lies outside the container's accessible range.
	/// @param anim Animation resource used by the operation.
	/// @return True when an index lies outside the container's accessible range; otherwise false.
	bool Animator::IndexOutOfRange(FTSpriteAnimation* anim)
	{
		return (anim->GetMaxFrameIdx() - anim->GetMinFrameIdx()) < mCurrFrameIdx;
	}

	/// @brief Initializes the services and state required before this object's runtime lifecycle begins.
	void Animator::Initialize()
	{
		if (0 < mLoadedAnim->GetSize())
			Play(0);

		D3D11Component::Initialize();
	}

	/// @brief Provides an empty lifecycle or extension hook for this implementation.
	/// @param inputDevice Device exposing the current frame's input state.
	void Animator::ProcessInput(IInputDevice* inputDevice)
	{
	}

	/// @brief Provides an empty lifecycle or extension hook for this implementation.
	/// @param deltaTime Elapsed frame time in seconds.
	void Animator::Update(float deltaTime)
	{
	}

	/// @brief Runs the post-update lifecycle phase after ordinary frame updates.
	/// @param deltaTime Elapsed frame time in seconds.
	void Animator::LateUpdate(float deltaTime)
	{
		if (!GetSprite())
			return;
		UpdateFrame(deltaTime);
	}

	/// @brief Submits this object's graphics work for the current frame.
	/// @param renderer Renderer providing the graphics device and current render state.
	/// @param camInst Camera supplying the view and projection for this draw.
	void Animator::Render(Core::IRenderer* renderer, Core::ICamera* camInst)
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

	/// @brief Copies this object's state into the requested target or attaches its clone to the target actor.
	/// @param actor Actor participating in this operation.
	void Animator::CloneTo(Core::IActor* actor)
	{
		Animator* newComp = DBG_NEW Animator(actor, GetUpdateOrder());

		newComp->mLoadedAnim->Assign(this->mLoadedAnim);
		newComp->SetSprite(this->GetSprite());
		newComp->SetPSO(this->GetPSO());
		newComp->SetMaterial(this->GetMaterial());
	}

#ifdef FOXTROT_EDITOR
	/// @brief Advances state needed only while operating in the editor.
	/// @param deltaTime Elapsed frame time in seconds.
	void Animator::EditorUpdate(float deltaTime)
	{
		LateUpdate(deltaTime);
	}

	/// @brief Updates the object's editor-facing controls.
	/// @param chInst Glyph instance whose metrics or vertices are used.
	void Animator::EditorUIUpdate(Editor::CommandHistory* chInst)
	{
		chInst->UpdateBoolValue("Is Repeated", mIsRepeated);

		if (GetSprite())
			GetSprite()->UpdateUI();

		Editor::DisplayResSelection(
			"Select Sprite",
			D3D11::ResourceManager::GetInstance()->GetSprites(),
			Sprite());

		if (GetMaterial())
			GetMaterial()->UpdateUI();

		Editor::DisplayResSelection(
			"Select Material",
			D3D11::ResourceManager::GetInstance()->GetMaterials(),
			Material());

		UpdatePlayAnim();
		UpdatePlayList();
	}

	/// @brief Submits editor-specific overlays and viewport graphics.
	/// @param renderer Renderer providing the graphics device and current render state.
	/// @param camInst Camera supplying the view and projection for this draw.
	void Animator::EditorRender(Core::IRenderer* renderer, Core::ICamera* camInst)
	{
		if (GetSprite())
		{
			// renderer->SetFillMode();

			Transform*		   transform = GetOwner()->GetTransform();
			FTSpriteAnimation* anim		 = static_cast<FTSpriteAnimation*>(GetSprite());
			GetSprite()->UpdateConstantBuffers(
				renderer,
				transform,
				camInst,
				GetMaterial(),
				anim->GetFrameCount(),
				GetCurrFrameIdx());

			GetSprite()->Render(
				renderer,
				transform,
				camInst,
				GetPSO(),
				GetMaterial());
		}
	}

	/// @brief Processes the editor controls for animation playback.
	void Animator::UpdatePlayAnim()
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

	/// @brief Refreshes the animation selection and playback list.
	void Animator::UpdatePlayList()
	{
		FTSpriteAnimation* anim = nullptr;
		Editor::DisplayResSelection<FTSpriteAnimation>(
			"Load Animation",
			D3D11::ResourceManager::GetInstance()->GetSpriteAnimations(),
			anim);

		if (anim)
		{
			mLoadedAnim->PushBack(anim);
			if (mLoadedAnim->GetSize() == 1)
				Sprite() = anim;
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
						D3D11::AnimationManager::GetInstance()->SaveAnimationAsFile(anim);

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

} // namespace D3D11
namespace D3D11 {
/// @brief Creates an animation component associated with an actor.
/// @param actor Actor participating in this operation.
/// @return Created animator instance or resource.
D3D11_API D3D11::Animator* CreateAnimator(Core::IActor* actor)
	{
		return DBG_NEW D3D11::Animator(actor);
	}
}
