#pragma once
#include "Script.h"

class ChangedScript : public Game::Script
{
public:
	static constexpr const char* NAME = "Script is changed";
	ChangedScript(Common::IActor* owner)
		: Script(owner, NAME)
	{
	}
	void Update(float deltaTime) override
	{
		printf("Spinning...");
	}
};

REGISTERED_SCRIPT(ChangedScript)