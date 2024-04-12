#pragma once
#include <string>

class Actor;
class AIComponent;

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
	AIComponent*		 GetOwner() { return mOwner; }

public:
	virtual void Update(float deltaTime) = 0;

public:
	AIState(class AIComponent* owner)
		: mOwner(owner)
	{}

private:
	AIComponent* mOwner;
};

class AIIdle :public AIState
{
public:
	virtual void OnEnter() override;
	virtual void OnExit() override;

public:
	AISTATE_TYPE GetType() { return AISTATE_TYPE::IDLE; }

public:
	virtual void Update(float deltaTime) override;

public:
	AIIdle(class AIComponent* owner)
		: AIState(owner)
	{}
};

class AIPatrol :public AIState
{
public:
	virtual void Update(float deltaTime) override;
	virtual void OnEnter() override;
	virtual void OnExit() override;

public:
	AISTATE_TYPE GetType() { return AISTATE_TYPE::PATROL; }

public:
	AIPatrol(class AIComponent* owner)
		: AIState(owner)
	{}
};

class AIPursue :public AIState
{
public:
	virtual void OnEnter() override;
	virtual void OnExit() override;

public:
	AISTATE_TYPE GetType() { return AISTATE_TYPE::PURSUE; }

public:
	virtual void Update(float deltaTime) override;

public:
	AIPursue(class AIComponent* owner);

private:
	Actor* mTargetActor;
};