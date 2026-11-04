// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2026 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Behavior/FTBehavior.h"

#include <fstream>

#include "Actor/IActor.h"
#include "Component/Data/ComponentData.h"
#include "FileSystem/FileIOHelper.h"

namespace Core
{
	/// @brief Initializes behavior state associated with its actor.
	/// @param owner Actor that owns this behavior.
	/// @param name Stable component name used as the chunk and factory key.
	/// @param updateOrder Relative order stored with the behavior.
	FTBehavior::FTBehavior(Common::IActor* owner, const char* name, int updateOrder)
		: mOwner(owner)
		, mName(name && name[0] ? name : "")
		, mUpdateOrder(updateOrder)
		, mIsActive(true)
		, mIsInitialized(false)
		, mIsSetup(false)
	{
	}

	/// @brief Completes destruction through the object's inheritance hierarchy.
	FTBehavior::~FTBehavior()
	{
	}

	/// @brief Initializes the services and state required before this object's runtime lifecycle begins.
	void FTBehavior::Initialize()
	{
	}

	/// @brief Prepares the behavior after initialization.
	void FTBehavior::Setup()
	{
	}

	/// @brief Dispatches input for the current frame to the relevant engine objects.
	/// @param inputDevice Device exposing the current frame's input state.
	void FTBehavior::ProcessInput(InputSystem::IInputDevice* inputDevice)
	{
		(void)inputDevice;
	}

	/// @brief Advances frame-dependent state using the current time step.
	/// @param deltaTime Elapsed frame time in seconds.
	void FTBehavior::Update(float deltaTime)
	{
		(void)deltaTime;
	}

	/// @brief Runs the post-update lifecycle phase after ordinary frame updates.
	/// @param deltaTime Elapsed frame time in seconds.
	void FTBehavior::LateUpdate(float deltaTime)
	{
		(void)deltaTime;
	}

	/// @brief Submits this object's graphics work for the current frame.
	/// @param renderer Renderer providing the graphics device and current render state.
	/// @param camInst Camera supplying the view and projection for this draw.
	void FTBehavior::Render(Graphics::IRenderer* renderer, Graphics::ICamera* camInst)
	{
		(void)renderer;
		(void)camInst;
	}

	/// @brief Serializes this object's persistent properties to a .chunk stream.
	/// @param ofs Output stream receiving the serialized data.
	void FTBehavior::SaveProperties(std::ofstream& ofs)
	{
		Common::FileIOHelper::SaveBool(ofs, Common::ChunkKey::IS_ACTIVE, mIsActive);
		Common::FileIOHelper::SaveInt(ofs, Common::ChunkKey::UPDATE_ORDER, mUpdateOrder);
	}

	/// @brief Restores this object's persistent properties from a .chunk stream.
	/// @param ifs Input stream positioned at the expected data; reading advances its position.
	void FTBehavior::LoadProperties(std::ifstream& ifs)
	{
		Common::FileIOHelper::LoadInt(ifs, mUpdateOrder);
		Common::FileIOHelper::LoadBool(ifs, mIsActive);
	}

	/// @brief Returns the name used by this behavior.
	/// @return Borrowed name.
	const char* FTBehavior::GetName()
	{
		return mName;
	}

	/// @brief Returns the owner used by this behavior.
	/// @return Borrowed owner, or nullptr.
	Common::IActor* FTBehavior::GetOwner()
	{
		return mOwner;
	}

	/// @brief Copies this behavior onto the target actor.
	/// @param actor Actor that receives the copy.
	void FTBehavior::CloneTo(Common::IActor* actor)
	{
		(void)actor;
	}

	/// @brief Copies active state and update order onto another behavior.
	/// @param dest Behavior that receives the copied state.
	void FTBehavior::CopyBaseStateTo(FTBehavior& dest) const
	{
		dest.mIsActive	   = mIsActive;
		dest.mUpdateOrder  = mUpdateOrder;
	}

#ifdef FOXTROT_EDITOR
	/// @brief Advances state needed only while operating in the editor.
	/// @param deltaTime Elapsed frame time in seconds.
	void FTBehavior::EditorUpdate(float deltaTime)
	{
		(void)deltaTime;
	}

	/// @brief Submits editor-specific overlays and viewport graphics.
	/// @param renderer Renderer providing the graphics device and current render state.
	/// @param camInst Camera supplying the view and projection for this draw.
	void FTBehavior::EditorRender(Graphics::IRenderer* renderer, Graphics::ICamera* camInst)
	{
		(void)renderer;
		(void)camInst;
	}

	/// @brief Updates the object's editor-facing controls.
	void FTBehavior::EditorUIUpdate()
	{
	}
#endif // FOXTROT_EDITOR
} // namespace Core
