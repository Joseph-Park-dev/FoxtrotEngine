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
			void SetImpulseQuantity(FTVector2 impulse);

public:
	virtual void Initialize(FTCore* coreInstance) override;
	virtual void Update(float deltaTime) override;
	virtual void LateUpdate(float deltaTime) override;

public:
	BulletBehavior(Actor* owner, int drawOrder, int updateOrder);

private:
	Rigidbody2DComponent* mRigidbody;
	FTVector2	mImpulseQuantity;
	float		mDuration;

#ifdef FOXTROT_EDITOR
public:
	void CloneTo(Actor* actor);
#endif
};

