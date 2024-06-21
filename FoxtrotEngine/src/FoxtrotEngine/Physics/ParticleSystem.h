#pragma once
#include "FoxtrotEngine/Core/SingletonMacro.h"

class FTVector2;

struct FTParticle
{
	FTVector2 pos;
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
	FTParticle	particles[MAXPARTICLES];
	FTVector2	initialPos;		// Spawning position
	float		initialSpeed;	// Initial Velocity or Force applied
	bool		isActive;
	int			activeCount;
};

class ParticleSystem
{
	SINGLETON(ParticleSystem);

public:
	void CreateExplosion(FTVector2 initPos, float initSpeed, int duration, float gravity, float angle);

public:
	void Update(float deltaTime);
	void Render(FoxtrotRenderer* renderer);

private:
	std::vector<FTParticleExplosion*> explosions;

private:
	bool UpdateParticleState(FTParticle* p, FTVector2 initPos, float deltaTime);
	void RemoveFinishedEffect(FTParticleExplosion* effect);
};