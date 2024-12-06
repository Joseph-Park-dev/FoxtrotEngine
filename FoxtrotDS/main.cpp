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
	FTDS::ArrayStack<TestDataPack*>arrayStack = FTDS::ArrayStack<TestDataPack*>();
	arrayStack.Reserve(7);
	printf("%zd\n", arrayStack.Capacity());
	printf("%zd\n", arrayStack.Size());

	for (int i = 0; i < 7; ++i)
		arrayStack.Push(DBG_NEW TestDataPack(i));

	printf("%d\n",arrayStack.Size());

	arrayStack.Reserve(10);
	printf("%zd\n", arrayStack.Capacity());
	printf("%zd\n", arrayStack.Size());
	arrayStack.Peek()->PrintValues(arrayStack.Size()-1);

	for (size_t i = arrayStack.Size(); 0 < i; --i)
		arrayStack.Pop()->PrintValues(arrayStack.Size() - 1);

	for (size_t i = 0; i < arrayStack.Size(); ++i)
		delete arrayStack.Data()[i];

	arrayStack.Clear();

	return 0;
}