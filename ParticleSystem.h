#pragma once
#include "SingletonMacro.h"

class Vector2;

struct FTParticle
{
	Vector2 pos;
	float initialSpeed;
	float initialAngle;
	float duration;  // In milliseconds
	float elapsedTime;
	// Color values for effect
	int	  red;
	int   green;
	int	  blue;
	float gravity;
	bool  isActive;
};

#define MAXPARTICLES 50

struct FTParticleExplosion
{
	FTParticle particles[MAXPARTICLES];
	Vector2    initialPos;		// Spawning position
	float	   initialSpeed;	// Initial Velocity or Force applied
	bool	   isActive;
	int		   activeCount;
};

class ParticleSystem
{
	SINGLETON(ParticleSystem);

public:
	void CreateExplosion(Vector2 initPos, float initSpeed, int duration, float gravity, float angle);

public:
	void Update(float deltaTime);
	void Render(FoxtrotRenderer* renderer);

private:
	std::vector<FTParticleExplosion*> explosions;

private:
	bool UpdateParticleState(FTParticle* p, Vector2 initPos, float deltaTime);
	void RemoveFinishedEffect(FTParticleExplosion* effect);
};