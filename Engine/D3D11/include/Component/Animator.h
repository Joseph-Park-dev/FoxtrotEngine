// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Takes care of all FTAnimations registered.
/// All types of animation are handled here as FTResource derived from
/// the same class, FTAnimation.
/// (sprite animation, spine animation, etc.)
/// </summary>

#pragma once
#include "Component/SpriteRenderer.h"

#include "TemplateFunctions.h"

namespace D3D11
{
	class FTSpriteAnimation;

	class Animator :
		public SpriteRenderer
	{
	public:
		static inline const char* NAME = "Animator";
		virtual const char*		  GetName() override
		{
			return "Animator";
		}

	public:
		void Play(const size_t idx, bool isRepeated = true);
		void Stop();

	public:
		bool GetIsFinished() const;
		int	 GetCurrFrameIdx() const;

		void SetFrame(int frameNumber);
		void SetIsFinished(bool val);

	public:
		virtual void Initialize() override;
		virtual void ProcessInput(Core::IInputDevice* inputDevice) override;
		virtual void Update(float deltaTime) override;
		void		 LateUpdate(float deltaTime) override;
		void		 Render(Core::IRenderer* renderer, Core::ICamera* camInst) override;

		virtual void CloneTo(Core::IActor* actor) override;

	public:
		Animator(
			Core::IActor* owner,
			int			  updateOrder = Core::DefaultVal::UPDATE_ORDER);
		~Animator() override;

	private:
		Common::FTDS::DynamicArray<D3D11::FTSpriteAnimation*>* mLoadedAnim;
		int													   mCurrFrameIdx;
		float												   mAccTime;
		bool												   mIsFinished;
		bool												   mIsRepeated;

	public:
		virtual void SaveProperties(std::ofstream& ofs) override;
		virtual void LoadProperties(std::ifstream& ifs) override;

	private:
		void UpdateFrame(float deltaTime);
		bool IndexOutOfRange(FTSpriteAnimation* anim);

#ifdef FOXTROT_EDITOR
	public:
		virtual void EditorUpdate(float deltaTime) override;
		virtual void EditorRender(Core::IRenderer* renderer, Core::ICamera* camInst) override;
		virtual void EditorUIUpdate(Editor::CommandHistory* chInst) override;

	private:
		void UpdatePlayAnim();
		void UpdatePlayList();

#endif // FOXTROT_EDITOR
	};

	namespace ChunkKey
	{
		namespace Animator
		{
			constexpr const char* NAME		  = "Animator";
			constexpr const char* LOADED_KEYS = "Loaded Keys";

		} // namespace Animator
	} // namespace ChunkKey

	#include "Plugin/D3D11Exports.h"
	D3D11_API D3D11::Animator* CreateAnimator(Core::IActor* actor)
	{
		return DBG_NEW D3D11::Animator(actor);
	}
} // namespace D3D11