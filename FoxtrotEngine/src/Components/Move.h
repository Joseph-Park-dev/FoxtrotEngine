// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Moves the character by changing the Rigidbody's attributes,
/// i.e. setting velocity and giving force to the body.
/// </summary>

#pragma once
#include "Components/Component.h"

#include "Core/TemplateFunctions.h"

class FTVector2;
class Rigidbody2D;

class Move :
    public Component
{
    enum Controllable;

public:
    virtual std::string GetName() const override
    {
        return "Move";
    }

public:
    float         GetForwardSpeed ()  const { return mForwardSpeed; }
    float         GetAngularSpeed ()  const { return mAngularSpeed; }
    float         GetJumpForce    ()  const { return mJumpForce; }
    Controllable  IsControllable  ()  const { return mIsControllable; }

    void  SetForwardSpeed   (float speed)       { mForwardSpeed = speed; }
    void  SetAngularSpeed   (float speed)       { mAngularSpeed = speed; }
    void  SetJumpForce      (float jumpForce)   { mJumpForce = jumpForce; }

public:
    virtual void Initialize (FTCore* coreInstance)   override;
    virtual void LateUpdate (float deltaTime)        override;

    virtual void CloneTo    (Actor* actor)           override;

public:
    Move(class Actor* owner, int drawOrder, int updateorder);

protected:
    void Accelerate (b2Vec2 currVel, const Steering* steering);
    void Jump       (b2Vec2 currVel, const Steering* steering);
    void SetIsGrounded();

protected:
    enum Controllable
    {
        NO,
        YES
    };

private:
    Rigidbody2D*   mRigidbody;
    b2QueryFilter           mGroundFilter;

    float                   mForwardSpeed;
    float                   mAngularSpeed;
    float                   mJumpForce;

    Controllable            mIsControllable;
    bool                    mIsGrounded;

public:
    virtual void SaveProperties(std::ofstream& ofs) override;
    virtual void LoadProperties(std::ifstream& ifs) override;

#ifdef FOXTROT_EDITOR
    virtual void EditorUIUpdate() override;
#endif // FOXTROT_EDITOR
};