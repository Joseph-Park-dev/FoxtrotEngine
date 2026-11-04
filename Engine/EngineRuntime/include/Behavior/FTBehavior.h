// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2026 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Component base for gameplay scripts.
/// Game::Script derives from this class. Custom scripts derive from Game::Script
/// and override the lifecycle methods they need.
/// </summary>

#pragma once
#include "Plugin/CoreExports.h"
#include "Component/IComponent.h"

#include <iosfwd>

namespace Common
{
	class IActor;
}

namespace Core
{
	namespace Behavior
	{
		constexpr int UPDATE_ORDER = 0;
	}

	class FTBehavior :
		public Common::IComponent
	{
	public:
		/// @brief Initializes behavior state associated with its actor.
		/// @param owner Actor that owns this behavior. The actor's component list owns the instance.
		/// @param name Stable component name. Chunk files and the script factory use this key.
		///        The pointer must remain valid for the behavior's lifetime.
		/// @param updateOrder Relative order stored with the behavior.
		CORE_API FTBehavior(Common::IActor* owner, const char* name, int updateOrder = Behavior::UPDATE_ORDER);
		/// @brief Completes destruction through the object's inheritance hierarchy.
		CORE_API virtual ~FTBehavior();

		FTBehavior(const FTBehavior&)			 = delete;
		FTBehavior& operator=(const FTBehavior&) = delete;

	public:
		/// @brief Initializes the services and state required before this object's runtime lifecycle begins.
		CORE_API virtual void Initialize() override;
		/// @brief Prepares the behavior after initialization.
		CORE_API virtual void Setup() override;
		/// @brief Dispatches input for the current frame to the relevant engine objects.
		/// @param inputDevice Device exposing the current frame's input state.
		CORE_API virtual void ProcessInput(InputSystem::IInputDevice* inputDevice) override;
		/// @brief Advances frame-dependent state using the current time step.
		/// @param deltaTime Elapsed frame time in seconds.
		CORE_API virtual void Update(float deltaTime) override;
		/// @brief Runs the post-update lifecycle phase after ordinary frame updates.
		/// @param deltaTime Elapsed frame time in seconds.
		CORE_API virtual void LateUpdate(float deltaTime) override;
		/// @brief Submits this object's graphics work for the current frame.
		/// @param renderer Renderer providing the graphics device and current render state.
		/// @param camInst Camera supplying the view and projection for this draw.
		CORE_API virtual void Render(Graphics::IRenderer* renderer, Graphics::ICamera* camInst) override;

	public:
		/// @brief Serializes this object's persistent properties to a .chunk stream.
		/// @param ofs Output stream receiving the serialized data.
		/// @note Chunk values load in reverse save order. Write derived values after this call,
		///       and read them before calling LoadProperties.
		CORE_API virtual void SaveProperties(std::ofstream& ofs) override;
		/// @brief Restores this object's persistent properties from a .chunk stream.
		/// @param ifs Input stream positioned at the expected data; reading advances its position.
		CORE_API virtual void LoadProperties(std::ifstream& ifs) override;

	public:
		/// @brief Returns the name used by this behavior.
		/// @return Borrowed name. Empty when no name was supplied.
		CORE_API virtual const char* GetName() override;
		/// @brief Returns the owner used by this behavior.
		/// @return Borrowed owner, or nullptr.
		CORE_API virtual Common::IActor* GetOwner() override;
		/// @brief Copies this behavior onto the target actor.
		/// @param actor Actor that receives the copy.
		/// @note The base implementation does not create a derived instance. Game::Script supplies that.
		CORE_API virtual void CloneTo(Common::IActor* actor) override;

		/// @brief Returns the stored update order.
		int GetUpdateOrder() const { return mUpdateOrder; }
		/// @brief Returns whether this behavior participates in updates.
		bool GetIsActive() const { return mIsActive; }
		/// @brief Enables or disables updates for this behavior.
		/// @param isActive Replacement active state.
		void SetIsActive(bool isActive) { mIsActive = isActive; }

		/// @brief Reports whether Initialize has already run.
		bool IsInitialized() const { return mIsInitialized; }
		/// @brief Records that Initialize has run.
		void MarkInitialized() { mIsInitialized = true; }
		/// @brief Reports whether Setup has already run.
		bool IsSetup() const { return mIsSetup; }
		/// @brief Records that Setup has run.
		void MarkSetup() { mIsSetup = true; }

	protected:
		/// @brief Copies active state and update order onto another behavior.
		/// @param dest Behavior that receives the copied state.
		CORE_API void CopyBaseStateTo(FTBehavior& dest) const;

#ifdef FOXTROT_EDITOR
	public:
		/// @brief Advances state needed only while operating in the editor.
		/// @param deltaTime Elapsed frame time in seconds.
		CORE_API virtual void EditorUpdate(float deltaTime) override;
		/// @brief Submits editor-specific overlays and viewport graphics.
		/// @param renderer Renderer providing the graphics device and current render state.
		/// @param camInst Camera supplying the view and projection for this draw.
		CORE_API virtual void EditorRender(Graphics::IRenderer* renderer, Graphics::ICamera* camInst) override;
		/// @brief Updates the object's editor-facing controls.
		CORE_API virtual void EditorUIUpdate() override;
#endif // FOXTROT_EDITOR

	private:
		Common::IActor* mOwner;
		const char*		mName;
		int				mUpdateOrder;
		bool			mIsActive;
		bool			mIsInitialized;
		bool			mIsSetup;
	};
} // namespace Core
