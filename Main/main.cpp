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

#include "FTCore.h"

constexpr const wchar_t* DLL_PATH  = L"D:/2024_01/FoxtrotEngine_DirectX/x64/Foxtrot_Editor_Debug/Core.dll";
constexpr const char*	 PROC_NAME = PluginKey::FTCore::GET_INSTANCE;
using CORE_CONSTRUCTOR			   = FTCore* (*)();

int main(int argc, char* argv[])
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	HMODULE mod = LoadLibrary(DLL_PATH);

	CORE_CONSTRUCTOR getCoreInst	 = (CORE_CONSTRUCTOR)GetProcAddress(mod, PluginKey::FTCore::GET_INSTANCE);
	CORE_CONSTRUCTOR destroyCoreInst = (CORE_CONSTRUCTOR)GetProcAddress(mod, PluginKey::FTCore::DESTROY);

	FTCore* core	= getCoreInst();
	bool	success = core->Initialize();
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