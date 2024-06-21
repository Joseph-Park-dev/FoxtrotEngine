#pragma once
#include "FoxtrotEngine/Components/Component.h"

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
    virtual void Update(float deltaTime) override;

public:
    GunFiringComponent(Actor* owner, int drawOrder = DEFAULT_DRAWORDER, int updateorder = DEFAULT_UPDATEORDER);
    GunFiringComponent(Actor* owner, int drawOrder, int updateorder, MOUSE triggerKey, float delay);
    ~GunFiringComponent();

public:
    virtual void SaveProperties(std::ofstream& ofs) override;
    virtual void LoadProperties(std::ifstream& ifs) override;

private:
    MOUSE mTriggerKey;
    float mDelay;
    float mCurrentTick;
};

