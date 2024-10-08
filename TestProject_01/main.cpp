// ----------------------------------------------------------------
// Foxtrot Engine (2D/3D)
// Copyright (C) 2024 JungBae Park. All rights reserved.
// ----------------------------------------------------------------

/// <FTCore_Editor>
/// Editor fields & functions for development
/// Displays editor window, Provides functionality to create & edit .chunk files.
/// </FTCore_Editor>

#include "Core/FTCore.h"
#pragma comment	(lib, "FoxtrotEngine.lib")

int main(int argc, char* argv[])
{
	FTCore::GetInstance();
	bool success = FTCore::GetInstance()->Initialize();
	if (success) {
		printf("Initialization Complete\n");
		FTCore::GetInstance()->RunLoop();
	}
	else {
		printf("main() -> Initialization failed");
		return -1;
	}
	FTCore::GetInstance()->ShutDown();
	return 0;
}