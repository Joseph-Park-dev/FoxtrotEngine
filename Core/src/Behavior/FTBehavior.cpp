#include "FTBehavior.h"

#include "Actor/Actor.h"

FTBehavior::FTBehavior(Plugin* plugin, Actor* owner, int updateOrder)
	: Component(plugin, owner, updateOrder)
{}

FTBehavior::~FTBehavior()
{}