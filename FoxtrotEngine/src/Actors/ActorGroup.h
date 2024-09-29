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
		const char* ActorGroup[(unsigned int)ActorGroup::END] =
		{
			"DEFAULT",
			"PLAYER",
			"ENEMY",
			"GROUND",
			"UI"
		};
		return ActorGroup[(unsigned int)group];
	}

	static const char* GetActorGroupStr(int index)
	{
		const char* ActorGroup[(unsigned int)ActorGroup::END] =
		{
			"DEFAULT",
			"PLAYER",
			"ENEMY",
			"GROUND",
			"UI"
		};
		return ActorGroup[index];
	}
};