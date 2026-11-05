#pragma once
#include "Script.h"

class HelloWorld : public Game::Script
{
public:
	static constexpr const char* NAME = "HelloWorld";
	HelloWorld(Common::IActor* owner)
		: Script(owner, NAME)
	{
	}
	void Update(float deltaTime) override
	{
		printf("Spinning...");
	}
};

REGISTERED_SCRIPT(HelloWorld)