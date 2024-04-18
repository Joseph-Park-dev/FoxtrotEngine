#pragma once
#include <string>

#include "Actor.h"
#include"TemplateFunctions.h"

class KeyInputManager;
class Rigidbody2DComponent;
class AnimatorComponent;
class GunFiringComponent;
class Scene;

class Ship : public Actor
{
public:
	enum PLAYER_STATE
	{
		IDLE, WALKING_LEFT, WALKING_RIGHT,JUMPING,DEAD,
	};

private:
	PLAYER_STATE mCurrentState;
	PLAYER_STATE mPreviousState;
	int			 mDirection;

	Rigidbody2DComponent* rb;
	AnimatorComponent*	  ac;
	GunFiringComponent*	  gun;

public:
	void InitializeActor() override;
	void UpdateActor(float deltaTime) override;

public:
	Ship(Scene* scene);
	CLONE(Ship);

private:
	//void UpdateState();
	void UpdateAnimation();
};