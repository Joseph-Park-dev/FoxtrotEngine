// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// A signleton class that manages all particle effects.
/// Features will be added in the future versions.
/// </summary>

#pragma once
#include "Core/SingletonMacro.h"

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