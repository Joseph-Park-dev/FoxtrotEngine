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
    virtual std::wstring GetName() const override { return L"GunFiringComponent"; }
    float                GetDelay() { return mDelay; }
    float                SetDelay(float delay) { mDelay = delay; }

public:
    virtual void Initialize(FTCore* coreInstance) override {};
    virtual void Update(float deltaTime) override;

public:
    GunFiringComponent(Actor* owner, int drawOrder = DEFAULT_DRAWORDER, int updateorder = DEFAULT_UPDATEORDER);
    GunFiringComponent(Actor* owner, int drawOrder, int updateorder, MOUSE triggerKey, float delay);
    ~GunFiringComponent();

public:
    virtual void SaveProperties(nlohmann::ordered_json& out) override;
    virtual void LoadProperties(std::ifstream& ifs) override;

private:
    MOUSE mTriggerKey;
    float mDelay;
    float mCurrentTick;
};

