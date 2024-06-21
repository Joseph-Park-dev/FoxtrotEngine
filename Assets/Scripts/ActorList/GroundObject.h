#pragma once
#include "FoxtrotEngine/Actors/Actor.h"
#include "FoxtrotEngine/Core/TemplateFunctions.h"

class GroundObject :
    public Actor
{
public:
    virtual void OnCollisionEnter(ColliderComponent* other) override;
    virtual void OnCollisionStay(ColliderComponent* other) override;
    virtual void OnCollisionExit(ColliderComponent* other) override;
    virtual void OnRayEnter() override;

public:
    virtual void UpdateActor(float deltaTime) override;

public:
    GroundObject(class Scene* scene);
    ~GroundObject();
    void Initialize();


private:
    Bounds             rect;
    ColliderComponent* collider;

private:
    void DirectionalBlocking(ColliderComponent* other);
};

