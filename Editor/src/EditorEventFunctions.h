#pragma once
#include "Math/FTMath.h"

namespace Core
{
	enum class ActorGroup;
}

namespace Editor
{
	class EditorElement;

	EditorElement* Instantiate(Actor* actor, Core::ActorGroup actorGroup, Math::FTVector3 pos);
	EditorElement* Instantiate(const char* premadeName);
	void		   Destroy(EditorElement* actor);
} // namespace Editor