#pragma once
#include "Components/Component.h"

#include "Actors/Transform.h"
#include "Components/CharacterAI/Steering.h"

class AgentBehavior :
    public Component
{
public:
    virtual Steering CreateSteering();

protected:
    Transform*  GetTarget() { return mTarget; }

    void    SetTarget(Transform* target);

public:
    void Initialize (FTCore* ftCoreInstance) override;
    void Setup      ()                       override;
    void Update     (float deltaTime)        override;

public:
    AgentBehavior(Actor* owner, int drawOrder, int updateOrder);
    ~AgentBehavior() override;

private:
    Transform* mTarget;
};