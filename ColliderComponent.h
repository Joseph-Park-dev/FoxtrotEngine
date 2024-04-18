#pragma once
#include "Component.h"
#include "TemplateFunctions.h"

class Vector2;

enum class CollidedSide
{
    NONE, LEFT, RIGHT, UP, DOWN
};

class ColliderComponent :
    public Component
{
public:
    Vector2      GetOffsetPos()     { return mOffsetPos; }
    // Get world position (anchorpoint at the center)
    Vector2      GetFinalPosition() { return mFinalPosition; }
    Vector2      GetScale()         { return mScale; }
    CollidedSide GetCollidedSide()  { return mCollidedSide; }
    uint32_t     GetID()            { return mID; }
    std::wstring GetName() const override
    {
        return L"ColliderComponent";
    }

    Vector2* GetOffsetPosRef()      { return &mOffsetPos; }
    Vector2* GetScaleRef()          { return &mScale; }

    void SetOffsetPos(Vector2 offset) { mOffsetPos = offset; }
    void SetScale(Vector2 scale) { mScale = scale; }

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

private:
    void OnCollisionEnter(ColliderComponent* other);
    void OnCollisionStay (ColliderComponent* other);
    void OnCollisionExit (ColliderComponent* other);

    void OnRayEnter();

    friend class CollisionManager;
    friend class CActor;

private:
    Vector2       mOffsetPos;
    Vector2       mFinalPosition;
    Vector2       mScale;
    CollidedSide  mCollidedSide;

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