#include "Spin.h"

#include "InputSystem/IInputDevice.h"

HelloWorld::HelloWorld(Common::IActor* owner)
	: Script(owner, NAME)
	, mMove(MOVE::STOP)
{
}

void HelloWorld::ProcessInput(InputSystem::IInputDevice* inputDevice)
{
	if (inputDevice->KEY_HOLD(InputSystem::KEYBOARD::A))
		mMove = MOVE::LEFT;
	else if (inputDevice->KEY_HOLD(InputSystem::KEYBOARD::D))
		mMove = MOVE::RIGHT;
	else if (inputDevice->KEY_HOLD(InputSystem::KEYBOARD::SPACE))
		mMove = MOVE::JUMP;
	else
		mMove = MOVE::STOP;
}

void HelloWorld::Update(float deltaTime)
{
	switch (mMove)
	{
		case MOVE::STOP:
			break;
		case MOVE::LEFT:
			printf("LEFT\n");
			break;
		case MOVE::RIGHT:
			printf("RIGHT\n");
			break;
		case MOVE::JUMP:
			printf("JUMP\n");
			break;
		default:
			break;
	}
}
