#pragma once
#include "Component.h"
#include "TemplateFunctions.h"

class Vector2;
class Actor;

class Rigidbody2DComponent :
    public Component
{
public:
    void AddForce(Vector2 force);
    void AddVelocity(Vector2 velocity);

public:
    float        GetMass() { return mMass; }
    std::wstring GetName() const override
    {
        return L"Rigidbody2DComponent";
    }
    bool GetIsGrounded() { return mIsGrounded; }
    bool GetIsBlockedUp() { return mIsBlockedUp; }
    void SetMass(float mass) { mMass = mass; }
    void SetVelocity(Vector2 velocity) { mVelocity = velocity; }
    void SetMaxVelocity(Vector2 velocity) { mMaxVelocity = velocity; }
    void SetIsGrounded(bool val) { mIsGrounded = val; }
    void SetIsBlockedUp(bool val) { mIsBlockedUp = val; }
    void SetAccelAlpha(Vector2 accel) { mAccelerationA = accel; }

public:
    virtual void Update(float deltaTime) override {};
    virtual void LateUpdate(float deltaTime) override;

public:
    Rigidbody2DComponent(Actor* owner, int drawOrder = DEFAULT_DRAWORDER, int updateOrder = DEFAULT_UPDATEORDER);
    ~Rigidbody2DComponent() {};

private:
    Vector2     mForce;
    Vector2     mAcceleration;
    Vector2     mAccelerationA;
    float       mMass;

    Vector2     mVelocity;
    Vector2     mMaxVelocity;
    float       mFrictionCoeff;
    bool        mIsGrounded;
    bool        mIsBlockedUp;

    // Box2D attributes
    //b2Body*     mBody;

private:
    void UpdateLinearPosition(float deltaTime);
    void UpdateAcceleration(float deltaTime);
    void UpdateGravity(float deltaTime);
    void UpdateVelocity(float deltaTime);
    void ClearForceAndAccel();

public:
    virtual void SaveProperties(std::ofstream& ofs) override;
    virtual void LoadProperties(std::ifstream& ifs) override;
};