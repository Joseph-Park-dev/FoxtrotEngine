#pragma once
#include <string>

#include "ActorGroup.h"
#include "SceneType.h"
#include "AIState.h"

class Actor;
class AIComponent;

// Object Spawning & Destroying
void Instantiate(Actor* actor, ACTOR_GROUP actorGroup);
void Destroy(Actor* actor);

// Scene Management
void SwitchScene(SCENE_TYPE sceneType);

// AI Controls
void ChangeAIState(AIComponent* ai, AISTATE_TYPE stateType);