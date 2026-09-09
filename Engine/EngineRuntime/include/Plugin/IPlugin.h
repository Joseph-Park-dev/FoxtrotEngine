// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2026 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Interface for all Plugins.
/// A direct instantiation of this class is strictly prohibited.
/// </summary>
#pragma once

#include "Debugging/DebugFuncs.h"
#include "Component/IComponent.h"
#include "FTDS/Static/FTString.h"

namespace Common
{
	class Entity;
	class IActor;

	class IPlugin
	{
	public:
		/// @brief Registers a component for plugin-managed lifecycle processing.
		/// @param comp Component instance associated with the actor or plugin.
		virtual void RegisterComponent(Common::IComponent* comp) = 0;

	public:
		/// @brief Serializes this object's persistent properties to a .chunk stream.
		/// @param ofs Output stream receiving the serialized data.
		/// @note Writes to the supplied stream at its current position.
		virtual void SaveProperties(std::ofstream& ofs) = 0;
		/// @brief Restores this object's persistent properties from a .chunk stream.
		/// @param ifs Input stream positioned at the expected data; reading advances its position.
		/// @note Advances the stream position and updates the destination state.
		virtual void LoadProperties(std::ifstream& ifs) = 0;

		/// @brief Initializes the services and state required before this object's runtime lifecycle begins.
		virtual void Initialize() = 0;
		/// @brief Updates the up used by subsequent operations.
		virtual void Setup()	  = 0;

		/// @brief Dispatches input for the current frame to the relevant engine objects.
		virtual void ProcessInput()				 = 0;
		/// @brief Advances frame-dependent state using the current time step.
		/// @param deltaTime Elapsed frame time in seconds.
		virtual void Update(float deltaTime)	 = 0;
		/// @brief Runs the post-update lifecycle phase after ordinary frame updates.
		/// @param deltaTime Elapsed frame time in seconds.
		virtual void LateUpdate(float deltaTime) = 0;
		/// @brief Submits this object's graphics work for the current frame.
		virtual void Render()					 = 0;
		/// @brief Processes queued lifecycle and scene changes at the frame boundary.
		virtual void ProcessEvent()				 = 0;

		/// @brief Releases runtime services and resources during engine shutdown.
		virtual void ShutDown() = 0;

	public:
		/// @brief Looks up an optional named interface implemented by this plugin.
		/// @note Unnamed parameter (const char*): reserved by this interface or unused by this implementation.
		/// @return Borrowed interface pointer, or nullptr when the interface is unsupported.
		virtual void* QueryInterface(const char*) noexcept { return nullptr; }
		/// @brief Completes destruction through the object's inheritance hierarchy.
		virtual ~IPlugin() = default;

	protected:
		/// @brief Restores plugin manager state from the chunk stream.
		/// @param ifs Input stream positioned at the expected data; reading advances its position.
		virtual void LoadManagerData(std::ifstream& ifs)  = 0;
		/// @brief Restores plugin resource state from the chunk stream.
		/// @param ifs Input stream positioned at the expected data; reading advances its position.
		virtual void LoadResourceData(std::ifstream& ifs) = 0;
	};

	namespace ChunkKey
	{
		namespace Plugin
		{
			constexpr const char* PLUGIN_DATA		= "Plugin Data";
			constexpr const char* DLL_PATH			= "DLL Path";
			constexpr const char* COMP_CONSTRUCTORS = "Comp Constructors";
			constexpr const char* MANAGER_DATA		= "Manager Data";
		} // namespace Plugin
	} // namespace ChunkKey
} // namespace Common
