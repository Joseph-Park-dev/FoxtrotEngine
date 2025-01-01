// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Detects the user input, and changes the moving direction.
/// </summary>

#pragma once
#include "Components/MoveComponent.h"

#include "Core/TemplateFunctions.h"

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