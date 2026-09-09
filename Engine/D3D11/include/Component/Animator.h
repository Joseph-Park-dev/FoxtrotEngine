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

#ifdef FOXTROT_EDITOR
namespace Editor
{
	class CommandHistory;
}
#endif

namespace D3D11
{
	class FTSpriteAnimation;

	class Animator :
		public SpriteRenderer
	{
	public:
		/// @brief Starts or selects animation playback.
		/// @param idx Zero-based element index.
		/// @param isRepeated Whether playback repeats after its final frame.
		void Play(const size_t idx, bool isRepeated = true);
		/// @brief Stops animation playback.
		void Stop();

	public:
		/// @brief Returns the is finished used by this animator.
		/// @return Current value of the is finished flag.
		bool GetIsFinished() const;
		/// @brief Returns the curr frame idx used by this animator.
		/// @return Current curr frame idx.
		int	 GetCurrFrameIdx() const;

		/// @brief Updates the frame used by subsequent operations.
		/// @param frameNumber Replacement frame.
		void SetFrame(int frameNumber);
		/// @brief Updates the is finished used by subsequent operations.
		/// @param val Replacement is finished.
		void SetIsFinished(bool val);

	public:
		/// @brief Initializes the services and state required before this object's runtime lifecycle begins.
		virtual void Initialize();
		/// @brief Provides an empty lifecycle or extension hook for this implementation.
		/// @param inputDevice Device exposing the current frame's input state.
		virtual void ProcessInput(Core::IInputDevice* inputDevice);
		/// @brief Provides an empty lifecycle or extension hook for this implementation.
		/// @param deltaTime Elapsed frame time in seconds.
		virtual void Update(float deltaTime);
		/// @brief Runs the post-update lifecycle phase after ordinary frame updates.
		/// @param deltaTime Elapsed frame time in seconds.
		void		 LateUpdate(float deltaTime);
		/// @brief Submits this object's graphics work for the current frame.
		/// @param renderer Renderer providing the graphics device and current render state.
		/// @param camInst Camera supplying the view and projection for this draw.
		void		 Render(Core::IRenderer* renderer, Core::ICamera* camInst);

		/// @brief Copies this object's state into the requested target or attaches its clone to the target actor.
		/// @param actor Actor participating in this operation.
		virtual void CloneTo(Core::IActor* actor);

	public:
		/// @brief Initializes sprite-animation playback state for its owner.
		/// @param owner Actor or object associated with the new instance.
		/// @param updateOrder Order used when dispatching component updates.
		Animator(
			Core::IActor* owner,
			int			  updateOrder = Core::DefaultVal::UPDATE_ORDER);
		/// @brief Releases the resources managed by this instance during destruction.
		~Animator();

	private:
		Common::FTDS::DynamicArray<D3D11::FTSpriteAnimation*>* mLoadedAnim;
		int													   mCurrFrameIdx;
		float												   mAccTime;
		bool												   mIsFinished;
		bool												   mIsRepeated;

	public:
		/// @brief Serializes this object's persistent properties to a .chunk stream.
		/// @param ofs Output stream receiving the serialized data.
		/// @note Writes to the supplied stream at its current position.
		virtual void SaveProperties(std::ofstream& ofs) override;
		/// @brief Restores this object's persistent properties from a .chunk stream.
		/// @param ifs Input stream positioned at the expected data; reading advances its position.
		/// @note Advances the stream position and updates the destination state.
		virtual void LoadProperties(std::ifstream& ifs) override;

	private:
		/// @brief Advances the animation's current frame according to playback state.
		/// @param deltaTime Elapsed frame time in seconds.
		void UpdateFrame(float deltaTime);
		/// @brief Tests whether an index lies outside the container's accessible range.
		/// @param anim Animation resource used by the operation.
		/// @return True when an index lies outside the container's accessible range; otherwise false.
		bool IndexOutOfRange(FTSpriteAnimation* anim);

#ifdef FOXTROT_EDITOR
	public:
		/// @brief Advances state needed only while operating in the editor.
		/// @param deltaTime Elapsed frame time in seconds.
		virtual void EditorUpdate(float deltaTime);
		/// @brief Submits editor-specific overlays and viewport graphics.
		/// @param renderer Renderer providing the graphics device and current render state.
		/// @param camInst Camera supplying the view and projection for this draw.
		virtual void EditorRender(Core::IRenderer* renderer, Core::ICamera* camInst);
		/// @brief Updates the object's editor-facing controls.
		/// @param chInst Glyph instance whose metrics or vertices are used.
		virtual void EditorUIUpdate(Editor::CommandHistory* chInst);

	private:
		/// @brief Processes the editor controls for animation playback.
		void UpdatePlayAnim();
		/// @brief Refreshes the animation selection and playback list.
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
/// @brief Creates an animation component associated with an actor.
/// @param actor Actor participating in this operation.
/// @return Created animator instance or resource.
D3D11_API D3D11::Animator* CreateAnimator(Core::IActor* actor);
} // namespace D3D11
