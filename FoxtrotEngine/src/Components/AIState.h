// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// This defines the Actor AI state - How an Actor behaves.
/// WIP in the current version.
/// </summary>

#pragma once
#include <string>

class Actor;
class AI;

enum class AISTATE_TYPE
{
	IDLE,
	PATROL,
	PURSUE,
};

class AIState
{
public:
	virtual void OnEnter() = 0;
	virtual void OnExit() = 0;

public:
	virtual AISTATE_TYPE GetType() = 0;
	AI*		 GetOwner() { return mOwner; }

public:
	virtual void Update(float deltaTime) = 0;

public:
	AIState(class AI* owner)
		: mOwner(owner)
	{
	}

private:
	AI* mOwner;
};

class AIIdle : public AIState
{
public:
	virtual void OnEnter() override;
	virtual void OnExit() override;

public:
	AISTATE_TYPE GetType() { return AISTATE_TYPE::IDLE; }

public:
	virtual void Update(float deltaTime) override;

public:
	AIIdle(class AI* owner)
		: AIState(owner)
	{
	}
};

class AIPatrol : public AIState
{
public:
	virtual void Update(float deltaTime) override;
	virtual void OnEnter() override;
	virtual void OnExit() override;

public:
	AISTATE_TYPE GetType() { return AISTATE_TYPE::PATROL; }

public:
	AIPatrol(class AI* owner)
		: AIState(owner)
	{
	}
};

class AIPursue : public AIState
{
public:
	virtual void OnEnter() override;
	virtual void OnExit() override;

public:
	AISTATE_TYPE GetType() { return AISTATE_TYPE::PURSUE; }

public:
	virtual void Update(float deltaTime) override;

public:
	AIPursue(class AI* owner);

private:
	Actor* mTargetActor;
};