// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2026 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <stdio.h>

#include "Core/FTCore.h"
#include "Plugin/CoreExports.h"

extern "C"
{
	CORE_API Core::FTCore* GetCore();
	CORE_API void		   DestroyCore();
}

int main(int argc, char* argv[])
{
	(void)argc;
	(void)argv;
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	Core::FTCore* core = GetCore();
	if (!core || !core->Initialize())
	{
		printf("main() -> Initialization failed\n");
		DestroyCore();
		return -1;
	}

	core->RunLoop();
	core->ShutDown();
	DestroyCore();
	return 0;
}
