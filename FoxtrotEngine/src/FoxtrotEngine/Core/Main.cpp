// ----------------------------------------------------------------
// Foxtrot Engine (2D/3D)
// Copyright (C) 2024 JungBae Park. All rights reserved.
// ----------------------------------------------------------------

#ifdef _DEBUG
/// <FTCore_Editor>
/// Editor fields & functions for development -> Active in Debug build; 
/// Displays editor window, Provides functionality to create & edit .chunk files.
/// </FTCore_Editor>
#include "FoxtrotEditor/FTCoreEditor.h"
int main(int argc, char* argv[])
{
	bool success = FTCoreEditor::GetInstance()->Initialize();
	if (success)
	{
		FTCoreEditor::GetInstance()->RunLoop();
	}
	FTCoreEditor::GetInstance()->ShutDown();
	FTCoreEditor::GetInstance()->Release();
	return 0;
}
#else
/// <FTCore_Game>
/// Game fields & functions for development -> Active in Release build; 
/// Loads .chunk files, runs the actual game.
/// </FTCore_Editor>
#include "FTCore.h"
int main(int argc, char* argv[])
{
	bool success = FTCore::GetInstance()->Initialize();
	if (success)
	{
		FTCore::GetInstance()->RunLoop();
	}
	FTCore::GetInstance()->ShutDown();
	FTCore::GetInstance()->Release();
	return 0;
}
#endif



