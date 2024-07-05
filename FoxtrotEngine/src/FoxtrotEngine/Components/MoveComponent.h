#pragma once
#include "FoxtrotEngine/Components/Component.h"

#include "FoxtrotEngine/Core/TemplateFunctions.h"

class FTVector2;
class Rigidbody2DComponent;

class MoveComponent :
    public Component
{
protected:
    enum State
    {
        IDLE,
        MOVING_LEFT,
        MOVING_RIGHT,
        MOVING_UP,
        MOVING_DOWN
    };
public:
    void Accelerate(float& currentSpeed, float accel, float maxSpeed, int forwardDir);
    void Accelerate(FTVector2 velocity);
    void Deccelerate(float& currentSpeed, int& currentDir, float breakSpeed);
    //void Accelerate(FTVector2 velocity);
    // Temporarily commented for testing Rigidbody2D-AddForce
    // void Deccelerate(float& currentSpeed, int& currentDir, float breakSpeed);

public:
    float GetAngularSpeed() const { return mAngularSpeed; }
    float GetForwardSpeed() const { return mForwardSpeed; }
    std::wstring GetName() const override
    {
        return L"MoveComponent";
    }
    State GetMovingState() const { return mMovingState; }
    State GetPrevMovingState() const { return mPrevMovingState; }
    //Rigidbody2DComponent* GetRigidbody() const { return mRigidbody; }
    void  SetAngularSpeed(float speed) { mAngularSpeed = speed; }
    void  SetForwardSpeed(float speed) { mForwardSpeed = speed; }
    void  SetMovingState(State state) { mMovingState = state; }
    void  SetPrevMovingState(State state) { mPrevMovingState = state; }

public:
    virtual void Update(float deltaTime) override;

public:
    MoveComponent(class Actor* owner, int drawOrder, int updateorder);

private:
    //Rigidbody2DComponent* mRigidbody;
    FTVector3 mVelocity;
    float mAngularSpeed;
    float mForwardSpeed;
    State mMovingState;
    State mPrevMovingState;
};

