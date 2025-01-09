#pragma once
#include "Components/CharacterAI/AgentBehavior.h"

class Seek :
    public AgentBehavior
{
public:
    virtual std::string GetName() const override;

public:
    virtual Steering CreateSteering() override;

public:
    virtual void Initialize (FTCore* ftCoreInstance) override;
    virtual void Setup      ()                       override;
    
    virtual void CloneTo    (Actor* actor)           override;

public:
    Seek(Actor* owner, int drawOrder, int updateOrder);
};