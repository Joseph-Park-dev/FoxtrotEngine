// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Components/InputMoveComponent.h"

#include "Managers/KeyInputManager.h"
#include "Math/FTMath.h"
#include "Actors/Actor.h"
#include "Actors/Transform.h"
#include "Components/Rigidbody2DComponent.h"

InputMoveComponent::InputMoveComponent(Actor* owner, int drawOrder, int updateOrder)
	: MoveComponent(owner, drawOrder, updateOrder)
{}

void InputMoveComponent::ProcessInput(KeyInputManager* keyInputManager)
{
	if (KEY_HOLD(KEY::A))
		SetHorizontalDir(MoveComponent::HorizontalDir::LEFT);
	else if (KEY_HOLD(KEY::D))
		SetHorizontalDir(MoveComponent::HorizontalDir::RIGHT);
	else
		SetHorizontalDir(MoveComponent::HorizontalDir::IDLE);

	if (KEY_TAP(KEY::SPACE))
		SetVerticalDir(MoveComponent::VerticalDir::UP);
	else
		SetVerticalDir(MoveComponent::VerticalDir::GROUND);
}