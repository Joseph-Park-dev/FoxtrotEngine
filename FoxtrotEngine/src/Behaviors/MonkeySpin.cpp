#include "MonkeySpin.h"

#include "Actors/Actor.h"
#include "Actors/Transform.h"

void MonkeySpin::Update(float deltaTime)
{
	FTVector3 rotation = GetOwner()->GetTransform()->GetRotation();
	GetOwner()->GetTransform()->SetRotation(
		rotation + FTVector3::UnitY * deltaTime);
}

void MonkeySpin::CloneTo(Actor* actor)
{
	MonkeySpin* newComp = DBG_NEW MonkeySpin(actor, GetUpdateOrder());
}

MonkeySpin::MonkeySpin(Actor* owner, int updateOrder)
	: FTBehavior(owner, updateOrder)
{
}

void MonkeySpin::SaveProperties(std::ofstream& ofs)
{
	Component::SaveProperties(ofs);
}

void MonkeySpin::LoadProperties(std::ifstream& ifs)
{
	Component::LoadProperties(ifs);
}

