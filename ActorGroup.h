#pragma once

enum class ACTOR_GROUP
{
	DEFAULT,
	PLAYER,
	ENEMY,
	GROUND,
	UI,

	EDITOR,
	END // This can be increased at one's will
};

const char* ActorGroup[(Uint32)ACTOR_GROUP::END] =
{
	"DEFAULT",
	"PLAYER",
	"ENEMY",
	"GROUND",
	"UI"
};

const char* GetActorGroupStr(ACTOR_GROUP group)
{
	return ActorGroup[(Uint32)group];
}

std::unordered_map<std::string, ACTOR_GROUP> ActorGroupMap
{
	{ "DEFAULT", ACTOR_GROUP::DEFAULT },
	{ "PLAYER",  ACTOR_GROUP::PLAYER },
	{ "ENEMY",   ACTOR_GROUP::ENEMY },
	{ "GROUND",  ACTOR_GROUP::GROUND },
	{ "UI",		 ACTOR_GROUP::UI }
};