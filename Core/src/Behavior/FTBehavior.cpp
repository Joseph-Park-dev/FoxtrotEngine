#include "FTBehavior.h"

#include "Actor/Actor.h"

namespace Core
{
	FTBehavior::FTBehavior(Plugin* plugin, Actor* owner, int updateOrder)
		: Component(plugin, owner, updateOrder)
	{
	}

	FTBehavior::~FTBehavior()
	{
	}
} // namespace Core