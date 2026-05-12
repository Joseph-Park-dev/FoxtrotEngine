// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Main function of Foxtrot Editor.
/// </summary>

#pragma once
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <Windows.h>
#include <stdio.h>

#include "Plugin/IPlugin.h"
#include "FTDS/Static/HashMap.h"
#include "Plugin/PluginKey.h"
#include "DLLData.h"

constexpr Common::FTDS::HashMap<Core::Plugin*>* gPlugins;

int main(int argc, char* argv[])
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	HMODULE mod = LoadLibraryA(DLLPaths::CORE_EDITOR);

	FARPROC		   proc	   = GetProcAddress(mod, Core::PluginKey::CREATE_PLUGIN);
	Core::GET_CORE getCore = reinterpret_cast<Core::GET_CORE>(proc);

	bool success = core->Initialize();
	if (success)
		core->RunLoop();
	else
	{
		printf("main() -> Initialization failed");
		return -1;
	}
	core->ShutDown();
	destroyCoreInst();

	return 0;
}