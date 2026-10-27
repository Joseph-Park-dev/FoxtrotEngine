// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Collects component factories exported by the DLLs imported by the current project.
/// </summary>

#pragma once
#include "Utility/SingletonMacro.h"
#include "Factory/IComponentFactory.h"
#include "FTDS/Static/HashMap.h"

namespace Editor
{
	class ComponentRegistry
	{
		SINGLETON(ComponentRegistry)

	public:
		/// <summary>
		/// Borrows the map of component names to factory functions collected from imported DLLs.
		/// </summary>
		/// @brief Returns the component factories collected from imported DLLs.
		/// @return Borrowed map keyed by component name.
		/// @note The map owns its record nodes. Values are function pointers and must not be deleted.
		Common::FTDS::HashMap<ComponentFactoryEntry>& GetComponentFactories();

		/// <summary>
		/// Reads the project DLL list and asks each loaded module for the components it can create.
		/// </summary>
		/// @brief Loads component factories from the DLLs named in DLL List.ftconfig.
		void FetchFromImportedDLLs();

	private:
		Common::FTDS::HashMap<ComponentFactoryEntry> mComponentFactories;

		/// @brief Resolves one imported module and merges its component factories into the registry.
		/// @param name Module name without the .dll extension.
		void QueryImportedModule(const Common::FTDS::String& name);
	};
} // namespace Editor
