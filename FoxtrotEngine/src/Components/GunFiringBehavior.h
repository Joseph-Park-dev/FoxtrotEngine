// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "Components/Component.h"

#include <string>

#include "ResourceSystem/FTPremade.h"

class Actor;
enum class MOUSE;

class GunFiringBehavior :
    public Component
{
public:
    void ShootGun();

public:
    virtual std::string  GetName() const override { return "GunFiringBehavior"; }
    float                GetDelay() { return mDelay; }
    float                SetDelay(float delay) { mDelay = delay; }

	virtual void		 LoadProperties(std::ifstream& ifs) override;

public:
    virtual void Initialize(FTCore* coreInstance)               override;
    virtual void ProcessInput(KeyInputManager* keyInputManager) override;
    virtual void Update(float deltaTime)                        override;
    
    virtual void CloneTo(Actor* actor)                          override;

public:
    GunFiringBehavior(Actor* owner, int drawOrder = DEFAULT_DRAWORDER, int updateorder = DEFAULT_UPDATEORDER);
    GunFiringBehavior(Actor* owner, int drawOrder, int updateorder, MOUSE triggerKey, float delay);
    ~GunFiringBehavior();

private:
    MOUSE       mTriggerKey;
    float       mDelay;
    float       mCurrentTick;

    // These fields will be assembled to form an impulse quantity
    // (Muzzle velocity)
    FTVector2   mDirection;

private:
    FTVector2&& CalcDirection(FTVector2&& origin, FTVector2&& point);
};