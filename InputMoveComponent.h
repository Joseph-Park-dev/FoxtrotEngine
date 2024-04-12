#pragma once
#include "MoveComponent.h"
#include "TemplateFunctions.h"

class InputMoveComponent :
    public MoveComponent
{
public:
	// Getters/setters for private variables
	float GetMaxForward() const { return mMaxForwardSpeed; }
	float GetMaxAngular() const { return mMaxAngularSpeed; }
    float GetBreakSpeed() const { return mBreakSpeed; }
    std::wstring GetName() const override
    {
        return L"InputMoveComponent";
    }
	void  SetMaxForwardSpeed(float speed) { mMaxForwardSpeed = speed; }
	void  SetMaxAngularSpeed(float speed) { mMaxAngularSpeed = speed; }
    void  SetAccelerationForward(float accel) { mAccelerationForward = accel; }
    void  SetAccelerationAngular(float accel) { mAccelerationAngular = accel; }
    void  SetMaxBreakSpeed(float speed) { mBreakSpeed = speed; }

public:
    void ProcessInput(class KeyInputManager* keyInputManager) override;
    virtual void Update(float deltaTime) override;
    virtual void LateUpdate(float deltaTime) override;

    CLONE(InputMoveComponent);

public:
    InputMoveComponent(class Actor* owner, int drawOrder, int updateOrder);

private:
    void Rotate();
    void Move();

private:
    float mMaxForwardSpeed;
    float mMaxAngularSpeed;
    float mAccelerationForward;
    float mAccelerationAngular;
    float mBreakSpeed;
    int   currentDir;
};

