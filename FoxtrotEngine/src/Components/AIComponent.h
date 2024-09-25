#pragma once
#include "Components/Component.h"

#include <string>
#include <unordered_map>

#include "Core/TemplateFunctions.h"
#include "Components/AIState.h"

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

private:
    std::unordered_map<AISTATE_TYPE, class AIState*> mStateMap;
    class AIState* mCurrentState;
};

