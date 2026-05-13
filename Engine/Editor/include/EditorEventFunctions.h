#pragma once
#include "FTMath.h"

namespace Core
{
	enum class ActorGroup;
	class Actor;
}

namespace Editor
{
	class EditorElement;

	EditorElement* Instantiate(Core::Actor* actor, Core::ActorGroup actorGroup, Math::FTVector3 pos);
	void		   Destroy(EditorElement* actor);
} // namespace Editor