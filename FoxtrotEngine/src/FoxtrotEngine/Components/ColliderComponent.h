#pragma once
#include "Component.h"

#include "FoxtrotEngine/Core/TemplateFunctions.h"

class FTVector2;

enum class CollidedSide
{
    NONE, LEFT, RIGHT, UP, DOWN
};

class ColliderComponent :
    public Component
{
public:
    FTVector2      GetOffsetPos()     { return mOffsetPos; }
    // Get world position (anchorpoint at the center)
    FTVector2      GetFinalPosition() { return mFinalPosition; }
    FTVector2      GetScale()         { return mScale; }
    CollidedSide GetCollidedSide()  { return mCollidedSide; }
    uint32_t     GetID()            { return mID; }
    std::wstring GetName() const override
    {
        return L"ColliderComponent";
    }

    FTVector2* GetOffsetPosRef()      { return &mOffsetPos; }
    FTVector2* GetScaleRef()          { return &mScale; }

    void SetOffsetPos(FTVector2 offset) { mOffsetPos = offset; }
    void SetScale(FTVector2 scale) { mScale = scale; }

public:
    // Shallow copying is not needed (duplicated ID)
    ColliderComponent& operator =(ColliderComponent& origin) = delete;

public:
    //This can also be used as refreshing method.
    void LateUpdate(float deltaTime)    override;
#ifdef _DEBUG
    void Render(FoxtrotRenderer* renderer) override;
#endif

public:
    ColliderComponent(class Actor* owner, int drawOrder, int updateOrder);
    ColliderComponent(const ColliderComponent& origin);
    virtual void CloneTo(Actor* actor) override;

private:
    void OnCollisionEnter(ColliderComponent* other);
    void OnCollisionStay (ColliderComponent* other);
    void OnCollisionExit (ColliderComponent* other);

    void OnRayEnter();

    friend class CollisionManager;
    friend class CActor;

private:
    FTVector2    mOffsetPos;
    FTVector2    mFinalPosition;
    FTVector2    mScale;
    CollidedSide mCollidedSide;

    static  uint32_t g_nextID;
            uint32_t mID;
            uint32_t mColliCount;

public:
    virtual void SaveProperties(std::ofstream& ofs) override;
    virtual void LoadProperties(std::ifstream& ifs) override;
    //virtual void LoadProperties(std::ofstream& of);

    void EditorLateUpdate(float deltaTime) override;
    void EditorUIUpdate() override;
    
private:
    void UpdateOffsetPos();
    void UpdateScale();
};