#pragma once
#include "Components/MoveComponent.h"

#include "Core/TemplateFunctions.h"

enum class MoveState {
    Idle,
    LeftMove,
    RightMove,
    Jump
};

class Rigidbody2DComponent;

class InputMoveComponent :
    public MoveComponent
{
public:
    std::string GetName() const override
    {
        return "InputMoveComponent";
    }

public:
    void ProcessInput(class KeyInputManager* keyInputManager) override;

public:
    InputMoveComponent(class Actor* owner, int drawOrder, int updateOrder);
};