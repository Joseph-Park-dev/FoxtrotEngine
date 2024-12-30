#pragma once
#include "Components/Component.h"

#include "Core/TemplateFunctions.h"

class FTVector2;
class Rigidbody2DComponent;

class MoveComponent :
    public Component
{
    enum Controllable;
    enum HorizontalDir;
    enum VerticalDir;

public:
    virtual std::string GetName() const override
    {
        return "MoveComponent";
    }

public:
    float         GetForwardSpeed()  const { return mForwardSpeed; }
    float         GetAngularSpeed()  const { return mAngularSpeed; }
    Controllable  IsControllable()   const { return mIsControllable; }
    HorizontalDir GetHorizontalDir() const { return mHorizontalDir; }
    VerticalDir   GetVerticalDir()   const { return mVerticalDir; }

    void  SetForwardSpeed   (float speed)       { mForwardSpeed = speed; }
    void  SetAngularSpeed   (float speed)       { mAngularSpeed = speed; }
    void  SetHorizontalDir  (HorizontalDir dir) { mHorizontalDir = dir; }
    void  SetVerticalDir    (VerticalDir dir)   { mVerticalDir = dir; }

public:
    virtual void Initialize(FTCore* coreInstance) override;
    virtual void Update(float deltaTime) override;

public:
    MoveComponent(class Actor* owner, int drawOrder, int updateorder);

protected:
    void Accelerate();
    void Jump();
    void SetIsGrounded();

protected:
    enum Controllable
    {
        NO,
        YES
    };

    enum HorizontalDir
    {
        LEFT = -1,
        IDLE = 0,
        RIGHT = 1
    };

    enum VerticalDir
    {
        // DOWN    = -1,
        GROUND = 0,
        UP = 1
    };

private:
    Rigidbody2DComponent*   mRigidbody;
    b2QueryFilter           mGroundFilter;

    float                   mForwardSpeed;
    float                   mAngularSpeed;
    float                   mJumpForce;

    Controllable            mIsControllable;
    HorizontalDir           mHorizontalDir;
    VerticalDir             mVerticalDir;
    bool                    mIsGrounded;

public:
    virtual void SaveProperties(std::ofstream& ofs) override;
    virtual void LoadProperties(std::ifstream& ifs) override;

#ifdef FOXTROT_EDITOR
    virtual void EditorUIUpdate() override;
    virtual void CloneTo(Actor* actor) override;
#endif // FOXTROT_EDITOR
};