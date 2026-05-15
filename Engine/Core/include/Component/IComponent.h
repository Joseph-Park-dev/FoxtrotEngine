// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Pure virtual interface for component implementations.
/// Defines the component lifecycle contract without platform dependencies.
/// </summary>

#pragma once
#include <iosfwd>

namespace Common
{
	namespace FTDS
	{
		class String;
	}
} // namespace Common

namespace Core
{
	class Actor;
	class IRenderer;
	class IInputDevice;

	/// <summary>
	/// Abstract component interface defining the lifecycle contract.
	/// All components must implement these methods regardless of platform.
	/// </summary>
	class IComponent
	{
	public:
		virtual ~IComponent() = default;

		//////////////////////////////////////////
		////// Identity //////////////////////////
		//////////////////////////////////////////

		/// <summary>
		/// Gets the component type name for serialization and debugging.
		/// </summary>
		virtual Common::FTDS::String GetName() const = 0;

		//////////////////////////////////////////
		////// Lifecycle /////////////////////////
		//////////////////////////////////////////

		/// <summary>
		/// Called once after component creation to initialize resources.
		/// </summary>
		virtual void Initialize() = 0;

		/// <summary>
		/// Called after Initialize() to complete setup (e.g., after all components are added).
		/// </summary>
		virtual void Setup() = 0;

		/// <summary>
		/// Called each frame to process input events.
		/// </summary>
		/// <param name="inputDevice">The input device to query.</param>
		virtual void ProcessInput(IInputDevice* inputDevice) = 0;

		/// <summary>
		/// Called each frame for main update logic.
		/// </summary>
		/// <param name="deltaTime">Time elapsed since last frame in seconds.</param>
		virtual void Update(float deltaTime) = 0;

		/// <summary>
		/// Called each frame after Update() for post-update logic.
		/// </summary>
		/// <param name="deltaTime">Time elapsed since last frame in seconds.</param>
		virtual void LateUpdate(float deltaTime) = 0;

		/// <summary>
		/// Called each frame to render the component.
		/// </summary>
		/// <param name="renderer">The active renderer instance.</param>
		virtual void Render(IRenderer* renderer) = 0;

		/// <summary>
		/// Creates a copy of this component attached to the specified actor.
		/// </summary>
		/// <param name="actor">The actor to clone to.</param>
		virtual void CloneTo(Actor* actor) = 0;

		//////////////////////////////////////////
		////// Serialization /////////////////////
		//////////////////////////////////////////

		/// <summary>
		/// Saves component properties to a stream.
		/// </summary>
		/// <param name="ofs">Output file stream.</param>
		virtual void SaveProperties(std::ofstream& ofs) = 0;

		/// <summary>
		/// Loads component properties from a stream.
		/// </summary>
		/// <param name="ifs">Input file stream.</param>
		virtual void LoadProperties(std::ifstream& ifs) = 0;

		//////////////////////////////////////////
		////// State /////////////////////////////
		//////////////////////////////////////////

		/// <summary>
		/// Gets the owning actor.
		/// </summary>
		virtual Actor* GetOwner() const = 0;

		/// <summary>
		/// Gets the update order priority (lower values update first).
		/// </summary>
		virtual int GetUpdateOrder() const = 0;

		/// <summary>
		/// Returns true if Initialize() has been called.
		/// </summary>
		virtual bool GetIsInitialized() const = 0;

		/// <summary>
		/// Returns true if Setup() has been called.
		/// </summary>
		virtual bool GetIsSetup() const = 0;

		/// <summary>
		/// Returns true if the component is active in the game loop.
		/// </summary>
		virtual bool GetIsActive() const = 0;

		/// <summary>
		/// Sets the component active state.
		/// </summary>
		virtual void SetIsActive(bool isActive) = 0;
	};
} // namespace Core
