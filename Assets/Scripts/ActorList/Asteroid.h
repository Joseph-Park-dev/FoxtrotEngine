#pragma once
#include "FoxtrotEngine/Actors/Actor.h"

#include "FoxtrotEngine/Core/TemplateFunctions.h"

class Scene;

struct tMonsterInfo
{
	float HP;
	float Speed;
	float RecogRange;
	float AttacRange;
	float AttacDamage;
};

class Asteroid :
    public Actor
{
public:
	tMonsterInfo GetMonsterInfo() { return monsterInfo; }
	void		 SetMonsterInfo(tMonsterInfo info) { monsterInfo = info; }

public:
	virtual void UpdateActor(float deltaTime) override {};

private:
	tMonsterInfo monsterInfo;

public:
	Asteroid(Scene* scene);
    Asteroid(Scene* scene, tMonsterInfo info);
    ~Asteroid();

	virtual void OnRayEnter() override;
};