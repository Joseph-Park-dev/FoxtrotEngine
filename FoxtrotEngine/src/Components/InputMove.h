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
#include "Components/Move.h"

#include "Core/TemplateFunctions.h"

class Rigidbody2D;

class InputMove :
    public Move
{
public:
    std::string GetName() const override
    {
        return "InputMove";
    }

public:
            void ProcessInput   (class KeyInputManager* keyInputManager) override;
    virtual void CloneTo        (Actor* actor)                           override;

public:
    InputMove(class Actor* owner, int updateOrder);
};