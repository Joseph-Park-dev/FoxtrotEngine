#pragma once
#include "Scenes/Scene.h"

#include "Actors/UIs/UIActor.h"

class Scene_Start : public Scene
{
public:
	virtual void Enter() override;
	virtual void Exit() override;

	virtual void Update(float deltaTime) override;

private:
	virtual void LoadData();
};

