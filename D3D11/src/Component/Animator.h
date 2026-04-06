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

namespace Core
{
	class FoxtrotRenderer;
}

namespace D3D11
{
	class FTSpriteAnimation;

	class Animator :
		public SpriteRenderer
	{
	public:
		virtual Core::FTDS::String GetName() const override
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
		virtual void ProcessInput(Core::FTInputDevice* inputDevice) override;
		virtual void Update(float deltaTime) override;
		void		 LateUpdate(float deltaTime) override;
		void		 Render(Core::FoxtrotRenderer* renderer) override;

		virtual void CloneTo(Core::Actor* actor) override;

	public:
		Animator(
			Core::Plugin* plugin,
			Core::Actor*  owner,
			int			  updateOrder = Core::DefaultVal::UPDATE_ORDER);
		~Animator() override;

	private:
		Core::FTDS::DynamicArray<D3D11::FTSpriteAnimation*>* mLoadedAnim;
		int													 mCurrFrameIdx;
		float												 mAccTime;
		bool												 mIsFinished;
		bool												 mIsRepeated;

	public:
		virtual void SaveProperties(std::ofstream& ofs) override;
		virtual void LoadProperties(std::ifstream& ifs) override;

	private:
		void UpdateFrame(float deltaTime);
		bool IndexOutOfRange(FTSpriteAnimation* anim);

#ifdef FOXTROT_EDITOR
	public:
		virtual void EditorUpdate(float deltaTime) override;
		virtual void EditorUIUpdate() override;
		void		 EditorRender(FoxtrotRenderer* renderer) override;

	private:
		void UpdatePlayAnim();
		void UpdatePlayList();

#endif // FOXTROT_EDITOR
	};

	namespace ChunkKey
	{
		namespace FTSpriteAnimator
		{
			constexpr const char* LOADED_KEYS = "Loaded Keys";

		} // namespace FTSpriteAnimator
	} // namespace ChunkKey
} // namespace D3D11