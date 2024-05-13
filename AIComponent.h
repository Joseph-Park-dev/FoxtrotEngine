#pragma once
#include <string>
#include <unordered_map>

#include "Component.h"
#include "TemplateFunctions.h"

class AIState;

class AIComponent :
    public Component
{
public:
    void ChangeState(AISTATE_TYPE name);
    void RegisterState(class AIState* state);

public:
    std::wstring GetName() const override
    {
        return L"AIComponent";
    }

public:
    void Update(float deltaTime) override;

public:
    AIComponent(class Actor* owner, int drawOrder = DEFAULT_DRAWORDER, int updateOrder = DEFAULT_UPDATEORDER);
    virtual ~AIComponent() override;
    CLONE(AIComponent);

private:
    std::unordered_map<AISTATE_TYPE, class AIState*> mStateMap;
    class AIState* mCurrentState;
};

