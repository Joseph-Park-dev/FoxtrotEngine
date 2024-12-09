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
	FTDS::CircularQueue<int>circularQueue = FTDS::CircularQueue<int>();
	circularQueue.Reserve(7);
	printf("%zd\n", circularQueue.Capacity());
	printf("%zd\n", circularQueue.Size());

	for (int i = 0; i < 7; ++i)
		circularQueue.Enqueue(i);

	printf("%d\n", circularQueue.Size());

	circularQueue.Reserve(10);
	printf("%zd\n", circularQueue.Capacity());
	printf("%zd\n", circularQueue.Size());

	for (int i = 0; i < 3; ++i)
	{
		circularQueue.Enqueue(i);
	}
	printf("value : %d\n", circularQueue.Peek());

	for (size_t i = circularQueue.Size(); 0 < i; --i)
		printf("value : %d\n", circularQueue.Dequeue());

	circularQueue.Clear();

	return 0;
}