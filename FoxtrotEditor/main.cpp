// ----------------------------------------------------------------
// Foxtrot Engine (2D/3D)
// Copyright (C) 2024 JungBae Park. All rights reserved.
// ----------------------------------------------------------------

/// <FTCore_Editor>
/// Editor fields & functions for development
/// Displays editor window, Provides functionality to create & edit .chunk files.
/// </FTCore_Editor>
/// 

#include <crtdbg.h>
#include "FTCoreEditor.h"
#pragma comment	(lib, "FoxtrotEngine.lib")

int main(int argc, char* argv[])
{
	bool success = FTCoreEditor::GetInstance()->Initialize();
	if (success)
		FTCoreEditor::GetInstance()->RunLoop();
	else {
		printf("main() -> Initialization failed");
		return -1;
	}
	FTCoreEditor::GetInstance()->ShutDown();
	FTCoreEditor::GetInstance()->Destroy();
	_CrtDumpMemoryLeaks();
	return 0;
}