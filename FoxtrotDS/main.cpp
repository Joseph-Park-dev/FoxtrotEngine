// ----------------------------------------------------------------
// Foxtrot Engine (2D/3D)
// Copyright (C) 2024 JungBae Park. All rights reserved.
// ----------------------------------------------------------------

/// <Foxtrot DS>
/// Data structure implementation for Foxtrot Engine.
/// This project will be exported as .lib,
/// hence the main function below exists purely for testing purpose.
/// </Foxtrot DS>
///
#include "Array.h"

#define RESERVE_SIZE 7
#define INCREMENTED_RESERVE_SIZE RESERVESIZE + 3
#define PORTION_SIZE RESERVE_SIZE - 3
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <chrono>
#include <unordered_map>

#include "HashChainMap.h"
#include "FTString.h"

using namespace FTDS;

int main(int argc, char* argv[])
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	FTDS::String string("123456789");
	FTDS::String result;
	string.SubStr(result, 1, 3);

	printf("%s\n", result.C_Str());
	printf("%d\n", result.Length());
	printf("%d\n", result.Capacity());
	
	return 0;
}