// ----------------------------------------------------------------
// Foxtrot Engine (2D/3D)
// Copyright (C) 2024 JungBae Park. All rights reserved.
// ----------------------------------------------------------------

/// <FTCore_Editor>
/// Editor fields & functions for development
/// Displays editor window, Provides functionality to create & edit .chunk files.
/// </FTCore_Editor>

#pragma once
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

#ifdef _DEBUG
#define DBG_NEW DBG_NEW ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
//#define DBG_NEW DBG_NEW
#else
#define DBG_NEW DBG_NEW
#endif // DEBUG

#include "FTCoreEditor.h"
#pragma comment	(lib, "FoxtrotEngine.lib")

int main(int argc, char* argv[])
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	bool success = FTCoreEditor::GetInstance()->Initialize();
	if (success)
		FTCoreEditor::GetInstance()->RunLoop();
	else {
		printf("main() -> Initialization failed");
		return -1;
	}
	FTCoreEditor::GetInstance()->ShutDown();
	FTCoreEditor::GetInstance()->Destroy();
	return 0;
}