#pragma once

enum class ActorGroup
{
	DEFAULT,
	PLAYER,
	ENEMY,
	GROUND,
	UI,

	EDITOR,
	END // This can be increased at one's will
};

struct ActorGroupUtil
{
	static const char* GetActorGroupStr(ActorGroup group)
	{
		return ActorGroupString[(unsigned int)group];
	}

	static const char* GetActorGroupStr(int index)
	{
		return ActorGroupString[index];
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
		"DEFAULT",
		"PLAYER",
		"ENEMY",
		"GROUND",
		"UI"
	};
};