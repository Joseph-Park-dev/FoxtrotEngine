// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Base class for all Components.
///
/// [Design philosophy]
/// The Component controls its elements in an upper level,
/// and the element has the related operation.
///
/// Means of access to the elements (Getters & Setters) are not to be
/// exposed. Make them protected, or minimize the cases of them being public.
/// </summary>

#pragma once
#include <iosfwd>

namespace Graphics
{
	class IRenderer;
	class ICamera;
} // namespace Graphics

namespace InputSystem
{
	class IInputDevice;
} // namespace InputSystem

namespace Common
{
	class IActor;

	namespace DefaultVal
	{
		constexpr int UPDATE_ORDER = 100;
	}

	class IComponent
	{
	public:
		/// @brief Completes destruction through the object's inheritance hierarchy.
		virtual ~IComponent() = default;

		//////////////////////
		/// Initialization ///
		//////////////////////
	public:
		/// @brief Initializes the services and state required before this object's runtime lifecycle begins.
		virtual void Initialize() = 0;
		/// @brief Updates the up used by subsequent operations.
		virtual void Setup()	  = 0;

		////////////////
		/// Gameloop ///
		////////////////
	public:
		/// @brief Dispatches input for the current frame to the relevant engine objects.
		/// @param inputDevice Device exposing the current frame's input state.
		virtual void ProcessInput(InputSystem::IInputDevice* inputDevice)			   = 0;
		/// @brief Advances frame-dependent state using the current time step.
		/// @param deltaTime Elapsed frame time in seconds.
		virtual void Update(float deltaTime)										   = 0;
		/// @brief Runs the post-update lifecycle phase after ordinary frame updates.
		/// @param deltaTime Elapsed frame time in seconds.
		virtual void LateUpdate(float deltaTime)									   = 0;
		/// @brief Submits this object's graphics work for the current frame.
		/// @param renderer Renderer providing the graphics device and current render state.
		/// @param camInst Camera supplying the view and projection for this draw.
		virtual void Render(Graphics::IRenderer* renderer, Graphics::ICamera* camInst) = 0;

		/////////////////
		/// Chunk I/O ///
		/////////////////
	public:
		/// @brief Serializes this object's persistent properties to a .chunk stream.
		/// @param ofs Output stream receiving the serialized data.
		/// @note Writes to the supplied stream at its current position.
		virtual void SaveProperties(std::ofstream& ofs) = 0;
		/// @brief Restores this object's persistent properties from a .chunk stream.
		/// @param ifs Input stream positioned at the expected data; reading advances its position.
		/// @note Advances the stream position and updates the destination state.
		virtual void LoadProperties(std::ifstream& ifs) = 0;

		////////////////////////////
		/// Accessors / Mutators ///
		////////////////////////////
	public:
		/// @brief Returns the name used by this icomponent.
		/// @return Borrowed access to the name.
		virtual const char*		GetName()				 = 0;
		/// @brief Returns the owner used by this icomponent.
		/// @return Borrowed access to the owner.
		virtual Common::IActor* GetOwner()				 = 0;
		/// @brief Returns the update order used by this icomponent.
		/// @return Current update order.
		virtual const int		GetUpdateOrder()		 = 0;
		/// @brief Returns the is initialized used by this icomponent.
		/// @return Current value of the is initialized flag.
		virtual const bool		GetIsInitialized() const = 0;
		/// @brief Returns the is setup used by this icomponent.
		/// @return Current value of the is setup flag.
		virtual const bool		GetIsSetup() const		 = 0;
		/// @brief Returns the is active used by this icomponent.
		/// @return Current value of the is active flag.
		virtual const bool		GetIsActive() const		 = 0;

		/// @brief Updates the is active used by subsequent operations.
		/// @param isActive Replacement is active.
		virtual void SetIsActive(bool isActive) = 0;

		//////////////////////////////
		/// Constructors / Copying ///
		//////////////////////////////
	public:
		/// @brief Copies this object's state into the requested target or attaches its clone to the target actor.
		/// @param actor Actor participating in this operation.
		virtual void CloneTo(Common::IActor* actor) = 0;

		//		///////////////////////////////
		//		/// Editor Member Functions ///
		//		///////////////////////////////
		// #ifdef FOXTROT_EDITOR
		//	public:
		//		virtual void EditorUpdate(float deltaTime)									 = 0;
		//		virtual void EditorRender(Graphics::IRenderer* renderer, Graphics::ICamera* camInst) = 0;
		//		virtual void EditorUIUpdate()												 = 0;
		// #endif // FOXTROT_EDITOR
	};

	namespace ChunkKey
	{
		constexpr const char* COMPONENTS = "Components";
	}
} // namespace Common
