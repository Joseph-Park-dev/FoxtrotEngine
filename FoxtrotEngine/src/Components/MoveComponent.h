#pragma once
#include "Components/Component.h"

#include "Core/TemplateFunctions.h"

class FTVector2;
class Rigidbody2DComponent;

class MoveComponent :
    public Component
{
protected:
    enum Controllable
    {
        NO,
        YES
    };

    enum HorizontalDir
    {
        LEFT    = -1,
        IDLE    = 0,
        RIGHT   = 1
    };

    enum VerticalDir
    {
        // DOWN    = -1,
        GROUND  = 0,
        UP      = 1
    };

public:
    std::string GetName() const override
    {
        return "MoveComponent";
    }

public:
    void Accelerate();
    void Jump();

public:
    float GetAngularSpeed() const { return mAngularSpeed; }
    float GetForwardSpeed() const { return mForwardSpeed; }
    Controllable  IsControllable()   const { return mIsControllable; }
    HorizontalDir GetHorizontalDir() const { return mHorizontalDir; }
    VerticalDir   GetVerticalDir()   const { return mVerticalDir; }

    void  SetHorizontalDir  (HorizontalDir dir) { mHorizontalDir = dir; }
    void  SetVerticalDir    (VerticalDir dir)   { mVerticalDir = dir; }
    void  SetAngularSpeed   (float speed) { mAngularSpeed = speed; }
    void  SetForwardSpeed   (float speed) { mForwardSpeed = speed; }

public:
    virtual void Initialize(FTCore* coreInstance) override;
    virtual void Update(float deltaTime) override;

public:
    MoveComponent(class Actor* owner, int drawOrder, int updateorder);

private:
    Rigidbody2DComponent*   mRigidbody;

    float                   mForwardSpeed;
    float                   mJumpForce;
    float                   mAngularSpeed;

    Controllable            mIsControllable;
    HorizontalDir           mHorizontalDir;
    VerticalDir             mVerticalDir;

public:
    virtual void SaveProperties(std::ofstream& ofs) override;
    virtual void LoadProperties(std::ifstream& ifs) override;

#ifdef FOXTROT_EDITOR
    virtual void EditorUIUpdate() override;
    virtual void CloneTo(Actor* actor) override;
#endif // FOXTROT_EDITOR
};