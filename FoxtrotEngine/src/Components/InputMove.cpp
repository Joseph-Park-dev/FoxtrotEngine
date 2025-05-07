// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Components/InputMove.h"

#include "InputSystem/FTInputDevice.h"
#include "Math/FTMath.h"
#include "Actors/Actor.h"
#include "Actors/Transform.h"
#include "Components/Rigidbody2D.h"

InputMove::InputMove(Actor* owner, int updateOrder)
	: Move(owner, updateOrder)
{}

void InputMove::ProcessInput(FTInputDevice* inputDevice)
{
	Transform* transform = GetOwner()->GetTransform();
	Steering steering = Steering::Halt();
	
	if (inputDevice->KEY_HOLD(KEY::A))
		steering.Linear = FTVector3::NegUnitX;
	else if (inputDevice->KEY_HOLD(KEY::D))
		steering.Linear = FTVector3::UnitX;
	
	if(inputDevice->KEY_NONE(KEY::A) && inputDevice->KEY_NONE(KEY::D))
		steering.Linear = FTVector3::Zero;

	if (inputDevice->KEY_TAP(KEY::SPACE))
		steering.JumpTriggered = true;
	GetOwner()->GetTransform()->SetSteering(steering);
}

void InputMove::CloneTo(Actor* actor)
{
	InputMove* newComp = DBG_NEW InputMove(actor, GetUpdateOrder());
	newComp->SetForwardSpeed (GetForwardSpeed());
	newComp->SetAngularSpeed (GetAngularSpeed());
	newComp->SetJumpForce	 (GetJumpForce());
}