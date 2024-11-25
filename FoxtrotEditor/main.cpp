// ----------------------------------------------------------------
// Foxtrot Engine (2D/3D)
// Copyright (C) 2024 JungBae Park. All rights reserved.
// ----------------------------------------------------------------

/// <FTCore_Editor>
/// Editor fields & functions for development
/// Displays editor window, Provides functionality to create & edit .chunk files.
/// </FTCore_Editor>

#define _CRTDBG_MAP_ALLOC

#ifdef _DEBUG
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#else
#define DBG_NEW new
#endif // DEBUG


#include "FTCoreEditor.h"
#pragma comment	(lib, "FoxtrotEngine.lib")

int main(int argc, char* argv[])
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(159);

	bool success = FTCoreEditor::GetInstance()->Initialize();
	if (success)
		FTCoreEditor::GetInstance()->RunLoop();
	else {
		printf("main() -> Initialization failed");
		return -1;
	}
	FTCoreEditor::GetInstance()->ShutDown();

	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
	return 0;
}