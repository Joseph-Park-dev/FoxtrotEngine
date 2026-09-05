#pragma once
#include "FTMath.h"

namespace Common
{
	enum class ActorGroup;
	class IActor;
} // namespace Common

namespace Editor
{
	class EditorElement;

	EditorElement* Instantiate(Common::IActor* actor, Common::ActorGroup actorGroup, Math::FTVector3 pos);
	void		   Destroy(EditorElement* actor);
} // namespace Editor