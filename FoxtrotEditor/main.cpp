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

#include "FTCoreEditor.h"

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