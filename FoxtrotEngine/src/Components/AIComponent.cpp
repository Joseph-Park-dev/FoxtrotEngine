// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Components/AIComponent.h"

#include "Components/AIState.h"
#include "Core/TemplateFunctions.h"

void AIComponent::ChangeState(AISTATE_TYPE stateType)
{
	if (mCurrentState)
		mCurrentState->OnExit();
	mCurrentState = mStateMap[stateType];
	if (mCurrentState == nullptr)
	{
		LogInt("Could not find AIState %d in state map", (int)stateType);
		mCurrentState = nullptr;
	}
	mCurrentState->OnEnter();
}

void AIComponent::RegisterState(AIState* state)
{
	mStateMap.emplace(state->GetType(), state);
}

void AIComponent::Initialize(FTCore* coreInstance)
{
	Component::Initialize(coreInstance);
}

void AIComponent::Update(float deltaTime)
{
	if (mCurrentState)
		mCurrentState->Update(deltaTime);
}

void AIComponent::CloneTo(Actor* actor)
{
	CLONE_TO_NOT_IMPLEMENTED
}

AIComponent::AIComponent(Actor* owner, int drawOrder, int updateOrder)
	: Component(owner)
	, mCurrentState(nullptr)
{
}

AIComponent::~AIComponent()
{
	Safe_Delete_Map(mStateMap);
}