#pragma once

class Asteroid;
class FTVector2;

enum class MON_TYPE
{
	NORMAL,
	RANGE
};

class MonFactory
{
public:
	static Asteroid* CreateMonster(Scene* scene, FTVector2 pos, MON_TYPE type);
};