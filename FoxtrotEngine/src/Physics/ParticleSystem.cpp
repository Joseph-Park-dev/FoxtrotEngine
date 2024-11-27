#include "Physics/ParticleSystem.h"

#include <vector>

#include "Math/Random.h"
#include "Math/FTMath.h"

#include "Renderer/FoxtrotRenderer.h"

// InitPos : WorldPosition, not ScreenPosition
void ParticleSystem::CreateExplosion(FTVector2 initPos, float initSpeed, int duration, float gravity, float angle)
{
	FTParticleExplosion* explosion = DBG_NEW FTParticleExplosion;
	explosion->isActive = true;
	explosion->initialPos = initPos;
	explosion->initialSpeed = initSpeed;
	explosion->activeCount = MAXPARTICLES;

	for (int i = 0; i < explosion->activeCount; ++i)
	{
		explosion->particles[i].pos = explosion->initialPos;
		explosion->particles[i].initialSpeed = Random::GetFloatRange(initSpeed / 2, initSpeed);

		if (angle < 999)
		{
			int dir = Random::GetIntRange(0, 1);
			explosion->particles[i].initialAngle = -angle + dir * Random::GetFloatRange(0, 10);
		}
		else
			explosion->particles[i].initialAngle = Random::GetFloatRange(0, 360);
		float f = Random::GetFloatRange(80, 100) / 100.f;
		explosion->particles[i].duration = duration * f;
		explosion->particles[i].red = 255;
		explosion->particles[i].blue = 0;
		explosion->particles[i].green = 0;
		explosion->particles[i].elapsedTime = 0.f;
		explosion->particles[i].isActive = true;
		explosion->particles[i].gravity = gravity;
	}
	explosions.emplace_back(explosion);
}

void ParticleSystem::Update(float deltaTime)
{
	for (UINT i = 0; i < explosions.size(); ++i)
	{
		if (explosions[i]->isActive)
		{
			for (UINT j = 0; j < explosions[i]->activeCount; ++j)
			{
				if (explosions[i]->particles[j].isActive)
				{
					FTVector2 effectPos = explosions[i]->initialPos;
					explosions[i]->particles[j].isActive =
						UpdateParticleState(&explosions[i]->particles[j], effectPos, deltaTime);
				}
				else
				{
					--explosions[i]->activeCount;
				}
			}
			if (explosions[i]->activeCount <= 0)
			{
				explosions[i]->isActive = false;
			}
		}
	}
	for (UINT i = 0; i < explosions.size(); ++i)
	{
		if (!explosions[i]->isActive)
		{
			RemoveFinishedEffect(explosions[i]);
		}
	}
}

bool ParticleSystem::UpdateParticleState(FTParticle* p, FTVector2 initPos, float deltaTime)
{
	bool updating = true;
	p->elapsedTime += deltaTime;
	float t = p->elapsedTime;
	float x = initPos.x + p->initialSpeed * Math::Cos(Math::ToRadians(p->initialAngle)) * t;
	float y = initPos.y + p->initialSpeed * Math::Sin(Math::ToRadians(p->initialAngle)) * t
		+ (p->gravity * t * t) / 2.f;
	p->pos = FTVector2(x, y);

	if (p->duration <= p->elapsedTime)
		updating = false;
	else
		updating = true;
	return updating;
}

void ParticleSystem::RemoveFinishedEffect(FTParticleExplosion* effect)
{
	auto iter = std::find(explosions.begin(), explosions.end(), effect);
	if (iter != explosions.end())
	{
		std::iter_swap(iter, explosions.end() - 1);
		explosions.pop_back();
	}
}

void ParticleSystem::Render(FoxtrotRenderer* renderer)
{
	for (UINT i = 0; i < explosions.size(); ++i)
	{
		if (explosions[i]->isActive)
		{
			for (int j = 0; j < explosions[i]->activeCount; ++j)
			{
				if (explosions[i]->particles[j].isActive)
				{
					//explosions[i]->particles[j].pos =
					//	Camera2D::GetInstance()->ConvertWorldPosToScreen(explosions[i]->particles[j].pos);
					/*
					
						Alternative to
						SDL_SetRenderDrawColor
						(
							renderer,
							255, 0, 0, 255
						);
					
					*/
					
					/*

						Alternative to
						SDL_Rect rect = {};
						rect.x = explosions[i]->particles[j].pos.x;
						rect.y = explosions[i]->particles[j].pos.y;
						rect.w = 10;
						rect.h = 10;
						SDL_RenderDrawRect
						(
							renderer,
							&rect
						);

					*/
				}
			}
		}
	}
}

ParticleSystem::ParticleSystem()
	: explosions{}
{}

ParticleSystem::~ParticleSystem() 
{
	if (0 < explosions.size())
	{
		for (UINT i = 0; i < explosions.size(); ++i)
		{
			delete explosions[i];
		}
		explosions.clear();
	}
}