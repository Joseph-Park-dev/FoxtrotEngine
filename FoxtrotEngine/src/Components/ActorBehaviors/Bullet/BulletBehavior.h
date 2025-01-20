// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "Behaviors/FTBehavior.h"

#include "Components/Rigidbody2D.h"

class BulletBehavior :
    public FTBehavior
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
	BulletBehavior(Actor* owner, int updateOrder);

private:
	Rigidbody2D*		mRigidbody;
	FTVector2			mImpulseQuantity;
	FTVector2			mStartPosition;
	FTVector2			mDirection;
	float				mSpeed;

	float				mDuration;

#ifdef FOXTROT_EDITOR
public:
	void CloneTo(Actor* actor);
#endif
};