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
#include "TestHeaderBatch.h"

#define RESERVE_SIZE 7
#define INCREMENTED_RESERVE_SIZE RESERVESIZE + 3
#define PORTION_SIZE RESERVE_SIZE - 3
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

#include <vector>

struct TestDataPack
{
public:
	TestDataPack(int val) {
		value = val;
	}

	int value;

	void PrintValues(int idx)
	{
		printf("value at %d : %d\n", idx, value);
	}
};

int main(int argc, char* argv[])
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	FTDS::ArrayStack<int>arrayStack = FTDS::ArrayStack<int>();
	arrayStack.Reserve(7);
	printf("%zd\n", arrayStack.Capacity());
	printf("%zd\n", arrayStack.Size());

	for (int i = 0; i < 7; ++i)
		arrayStack.Push(i);

	printf("%d\n",arrayStack.Size());

	arrayStack.Reserve(3);
	printf("%zd\n", arrayStack.Capacity());
	printf("%zd\n", arrayStack.Size());

	for (size_t i = arrayStack.Size(); 0 < i; --i)
		printf("%d\n", arrayStack.Pop());

	arrayStack.Clear();
	return 0;
}