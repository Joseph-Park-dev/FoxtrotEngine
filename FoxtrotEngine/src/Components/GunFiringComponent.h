#pragma once
#include "Components/Component.h"

#include <string>

class Actor;
enum class MOUSE;

class GunFiringComponent :
    public Component
{
public:
    void ShootGun();

public:
    virtual std::string  GetName() const override { return "GunFiringComponent"; }
    float                GetDelay() { return mDelay; }
    float                SetDelay(float delay) { mDelay = delay; }

	virtual void		 LoadProperties(std::ifstream& ifs) override;

public:
    virtual void Initialize(FTCore* coreInstance) override {};
    virtual void Update(float deltaTime) override;

public:
    GunFiringComponent(Actor* owner, int drawOrder = DEFAULT_DRAWORDER, int updateorder = DEFAULT_UPDATEORDER);
    GunFiringComponent(Actor* owner, int drawOrder, int updateorder, MOUSE triggerKey, float delay);
    ~GunFiringComponent();

private:
    MOUSE mTriggerKey;
    float mDelay;
    float mCurrentTick;
};