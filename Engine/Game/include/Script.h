// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2026 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Base for custom gameplay scripts compiled into Game.dll.
/// Derive from Script, give the script a stable NAME, and register it with Script::Add.
/// While the game is playing, the host calls Initialize, Setup, Update, and LateUpdate.
/// </summary>

#pragma once
#include "Behavior/FTBehavior.h"
#include "Actor/ActorData.h"
#include "Factory/IComponentFactory.h"
#include "Foxtrot/Core/ModuleApi.h"

#include <type_traits>

namespace Common
{
	class IPlugin;
}

namespace Foxtrot
{
	struct RuntimeControl;
}

namespace Game
{
	class Script :
		public Core::FTBehavior
	{
	public:
		/// @brief Initializes a script and enrolls it for frame updates.
		/// @param owner Actor that owns this script.
		/// @param name Stable script name. Must match the key passed to Add.
		/// @param updateOrder Relative order stored with the script.
		Script(Common::IActor* owner, const char* name, int updateOrder = Core::Behavior::UPDATE_ORDER);
		/// @brief Removes this script from frame updates.
		~Script() override;

		/// @brief Registers a script type so actors and .chunk files can create it.
		/// @return True when the type was recorded. False when the name is empty, already used, or the registry is full.
		/// @note Call this once from the script's .cpp, after the derived class is complete:
		///       static const bool registered = Game::Script::Add<Spin>(Spin::NAME);
		///       Spin needs a public constructor that takes Common::IActor*.
		template <typename T>
		static bool Add(const char* name)
		{
			static_assert(std::is_base_of<Script, T>::value, "Custom scripts must derive from Game::Script");
			return Register(name, &CreateRegisteredComponent<T>);
		}

		/// @brief Creates a registered copy on the target actor and copies script state onto it.
		/// @param actor Actor that receives the copy.
		virtual void CloneTo(Common::IActor* actor) override;

	protected:
		/// @brief Copies derived fields onto a clone created by CloneTo.
		/// @param dest Clone that already has this script's base state.
		virtual void CopyFieldsTo(Script& dest);

		/// @brief Appends a manually constructed clone to the target actor.
		/// @param actor Actor that will own the clone.
		/// @param clone Script constructed for that actor.
		/// @note The clone constructor already enrolls it for updates.
		void AttachClone(Common::IActor* actor, Script* clone);

	private:
		static bool Register(const char* name, ComponentFactoryEntry create);
	};

	/// @brief Returns the plugin that advances registered scripts.
	/// @return Borrowed plugin pointer owned by the Game module.
	Common::IPlugin* GetScriptPlugin();

	/// @brief Registers the script plugin with the runtime.
	/// @param control Runtime control table borrowed from EngineRuntime.
	/// @return Status from the runtime attach call.
	Foxtrot::Status AttachScriptPlugin(Foxtrot::RuntimeControl* control);

	/// @brief Destroys the script plugin owned by the Game module.
	void DestroyScriptPlugin();
} // namespace Game
