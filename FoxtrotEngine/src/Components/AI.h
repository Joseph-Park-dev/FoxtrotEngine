// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Base class for Actor AI.
/// WIP in the current version.
/// </summary>

#pragma once
#include "Components/Component.h"

#include <string>
#include <unordered_map>

#include "Core/TemplateFunctions.h"
#include "Components/AIState.h"

class AIState;

class AI :
	public Component
{
public:
	virtual std::string GetName() const override { return "AI"; }

public:
	void ChangeState(AISTATE_TYPE name);
	void RegisterState(class AIState* state);

public:
	virtual void Initialize	(FTCore* coreInstance)	override;
			void Update		(float deltaTime)		override;

	virtual void CloneTo	(Actor* actor)			override;

public:
	AI(class Actor* owner, int updateOrder = DEFAULT_UPDATEORDER);
	virtual ~AI() override;

private:
	std::unordered_map<AISTATE_TYPE, class AIState*> mStateMap;
	class AIState*									 mCurrentState;
};
