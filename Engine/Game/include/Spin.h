#pragma once
#include "Script.h"

enum class MOVE
{
	STOP,
	LEFT,
	RIGHT,
	JUMP
};

class HelloWorld : public Game::Script
{
public:
	static constexpr const char* NAME = "HelloWorld";
	HelloWorld(Common::IActor* owner);

	void ProcessInput(InputSystem::IInputDevice* inputDevice) override;
	void Update(float deltaTime) override;

private:
	MOVE mMove;
};

REGISTERED_SCRIPT(HelloWorld)