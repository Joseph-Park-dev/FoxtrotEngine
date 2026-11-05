// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "ComponentRegistry.h"

#include <fstream>
#include <utility>

#include <Windows.h>

#include "Debugging/DebugFuncs.h"
#include "FileSystem/DLLPath.h"
#include "FileSystem/FileIOHelper.h"
#include "FileSystem/NullKeys.h"

namespace
{
	constexpr const char* DLL_LIST = ".\\DLL List.ftconfig";
}

namespace Editor
{
	/// @brief Returns the component factories collected from imported DLLs.
	/// @return Borrowed map keyed by component name.
	Common::FTDS::HashMap<ComponentFactoryEntry>& ComponentRegistry::GetComponentFactories()
	{
		return mComponentFactories;
	}

	/// @brief Resolves one imported module and merges its component factories into the registry.
	/// @param name Module name without the .dll extension.
	void ComponentRegistry::QueryImportedModule(const Common::FTDS::String& name)
	{
		if (name.IsEmpty())
			return;

		Common::FTDS::String fileName(name);
		fileName.Append(".dll");

		HMODULE module = GetModuleHandleA(fileName.C_Str());
		if (!module && name.Equal("D3D11"))
			module = GetModuleHandleA(Common::DLLPath::D3D11);
		if (!module)
			return;

		using SetComponentFactoryFn = void(__cdecl*)(Common::FTDS::HashMap<ComponentFactoryEntry>&);
		auto setFactory				= reinterpret_cast<SetComponentFactoryFn>(GetProcAddress(module, "SetComponentFactory"));
		if (!setFactory)
			return;

		setFactory(mComponentFactories);
	}

	/// @brief Loads component factories from the DLLs named in DLL List.ftconfig.
	void ComponentRegistry::FetchFromImportedDLLs()
	{
		mComponentFactories.Clear();
		bool queriedGame = false;
		auto query		 = [&](const Common::FTDS::String& name) {
			if (name.Equal("Game") || name.Equal("Game.dll"))
				queriedGame = true;
			QueryImportedModule(name);
		};

		std::ifstream dllList = std::ifstream(DLL_LIST);
		if (!dllList.is_open())
			Common::Debug::LogError(__LINE__, __FILE__, "Failed to read DLL List.ftconfig");
		else
		{
			try
			{
				std::pair<size_t, Common::FTDS::String> pack = Common::FileIOHelper::BeginDataPackLoad(dllList);
				// BeginDataPackLoad returns NULL OBJECT when the first line has no data-pack marker.
				if (pack.second.Equal(Common::ChunkKey::NullVal::NULL_OBJECT))
				{
					dllList.clear();
					dllList.seekg(0, std::ios::beg);

					Common::FTDS::String line;
					while (Common::FileIOHelper::GetLine(dllList, line))
					{
						query(line);
						line.Clear();
					}
				}
				else
				{
					Common::FTDS::String dll;
					for (size_t i = 0; i < pack.first; ++i)
					{
						Common::FileIOHelper::LoadBasicString(dllList, dll);
						query(dll);
						dll.Clear();
					}
				}
			}
			catch (...)
			{
				mComponentFactories.Clear();
				queriedGame = false;
				Common::Debug::LogError(__LINE__, __FILE__, "Failed to read DLL List.ftconfig");
			}
		}

		// Game behaviors live in Game.dll even when the DLL list omits that entry.
		if (!queriedGame)
		{
			Common::FTDS::String game("Game");
			QueryImportedModule(game);
		}
	}

	/// @brief Initializes the component factory map.
	ComponentRegistry::ComponentRegistry()
		: mComponentFactories(32)
	{
	}

	/// @brief Releases record nodes owned by the factory map.
	ComponentRegistry::~ComponentRegistry()
	{
	}
} // namespace Editor
