#include "AIComponent.h"
#include "AIState.h"
#include "TemplateFunctions.h"

AIComponent::AIComponent(Actor* owner, int drawOrder, int updateOrder)
	: Component(owner)
	, mCurrentState(nullptr)
{
}

AIComponent::~AIComponent()
{
	Safe_Delete_Map(mStateMap);
}

void AIComponent::Update(float deltaTime)
{
	if (mCurrentState)
		mCurrentState->Update(deltaTime);
}

void AIComponent::ChangeState(AISTATE_TYPE stateType)
{
	if(mCurrentState)
		mCurrentState->OnExit();
	mCurrentState = mStateMap[stateType];
	if(mCurrentState == nullptr)
	{
		SDL_Log("Could not find AIState %d in state map", stateType);
		mCurrentState = nullptr;
	}
	mCurrentState->OnEnter();
}

void AIComponent::RegisterState(AIState* state)
{
	mStateMap.emplace(state->GetType(), state);
}
