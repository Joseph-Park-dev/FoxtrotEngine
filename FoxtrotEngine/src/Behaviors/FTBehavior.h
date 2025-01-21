#pragma once
#include "Components/Component.h"

class Collider2D;

class FTBehavior :
    public Component
{
public:
    FTBehavior(Actor* owner, int updateOrder);
    virtual ~FTBehavior();

protected:
    virtual void OnCollisionEnter(Collider2D* other);
    virtual void OnCollisionStay(Collider2D* other);
    virtual void OnCollisionExit(Collider2D* other);
    friend class Collider2D;
};

