#pragma once
#include <string>

#include "Actors/ActorGroup.h"
#include "Scenes/SceneType.h"
#include "Components/AIState.h"
#include "ResourceSystem/FTPremade.h"

class Actor;
class AIComponent;

// Object Spawning & Destroying
void	Instantiate(Actor* actor, ActorGroup actorGroup);
Actor*	Instantiate(const char* premadeName);
void	Destroy(Actor* actor);

// Scene Management
void SwitchScene(SCENE_TYPE* sceneType);