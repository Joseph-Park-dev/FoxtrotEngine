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

	/// @brief Creates an actor instance and schedules it for the requested scene or group.
	/// @param actor Actor participating in this operation.
	/// @param actorGroup Actor group used for scene classification.
	/// @param pos Position or zero-based insertion index.
	/// @return Created instantiate instance or resource.
	EditorElement* Instantiate(Common::IActor* actor, Common::ActorGroup actorGroup, Math::FTVector3 pos);
	/// @brief Provides an empty lifecycle or extension hook for this implementation.
	/// @param actor Actor participating in this operation.
	void		   Destroy(EditorElement* actor);
} // namespace Editor
