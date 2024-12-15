#pragma once

#include "Core/TemplateFunctions.h"

enum class ActorGroup
{
	NOT_ASSIGNED	= 0X00000000,

	DEFAULT			= 0x00000001,
	PLAYER			= 0x00000002,
	ENEMY			= 0x00000003,
	GROUND			= 0x00000004,
	UI				= 0x00000005,

	END // This can be increased at one's will
};

struct ActorGroupUtil
{
	static const size_t GetCount()
	{
		return static_cast<size_t>(ActorGroup::END)-1;
	}

	static const char* GetActorGroupStr(ActorGroup group)
	{
		return ActorGroupString[(size_t)group];
	}

	static const char* GetActorGroupStr(size_t index)
	{
		return ActorGroupString.at(index + 1);
	}

	static ActorGroup GetActorGroup(std::string str) {
		auto it = std::find(ActorGroupString.begin(), ActorGroupString.end(), str);
		if (it != ActorGroupString.end())
		{
			int index = it - ActorGroupString.begin();
			return (ActorGroup)index;
		}
		else {
			LogString("ERROR : GetActorGroup()-> cannot find the actorgroup");
			return ActorGroup::DEFAULT;
		}
	}

	static inline std::vector<const char*> ActorGroupString =
	{
		"NOT_ASSIGNED",
		"DEFAULT",
		"PLAYER",
		"ENEMY",
		"GROUND",
		"UI"
	};
};