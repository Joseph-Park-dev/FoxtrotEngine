#pragma once

class Asteroid;
class Vector2;

enum class MON_TYPE
{
	NORMAL,
	RANGE
};

class MonFactory
{
public:
	static Asteroid* CreateMonster(Scene* scene, Vector2 pos, MON_TYPE type);
};