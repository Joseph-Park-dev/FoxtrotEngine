#pragma once
#include "Components/Component.h"

#include "Components/Rigidbody2DComponent.h"

class BulletBehavior :
    public Component
{
public:
	virtual std::string  GetName() const { return "BulletBehavior"; }

public:
			void Thrust();

public:
	void SetStartPosition(FTVector2 pos);
	void SetImpulseQuantity(FTVector2 impulse);
	void SetDirection(FTVector2 direction);
	void SetSpeed(float speed);

public:
	virtual void Initialize(FTCore* coreInstance)	override;
	virtual void Setup()							override;
	virtual void Update(float deltaTime)			override;
	virtual void LateUpdate(float deltaTime)		override;

public:
	BulletBehavior(Actor* owner, int drawOrder, int updateOrder);

private:
	Rigidbody2DComponent* mRigidbody;
	FTVector2			  mImpulseQuantity;
	FTVector2			  mStartPosition;
	FTVector2			  mDirection;
	float				  mSpeed;

	float				  mDuration;

#ifdef FOXTROT_EDITOR
public:
	void CloneTo(Actor* actor);
#endif
};

