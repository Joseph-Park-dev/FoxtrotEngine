#pragma once
#include "Components/Component.h"

#include "Actors/Transform.h"
#include "Components/CharacterAI/Steering.h"

class AgentBehavior :
    public Component
{
public:
    void Initialize (FTCore* ftCoreInstance) override;
    void Setup      ()                       override;
    void Update     (float deltaTime)        override;

public:
    AgentBehavior();
    ~AgentBehavior() override;

private:
    Actor*      mTarget;
    Transform*  mAgent;
};