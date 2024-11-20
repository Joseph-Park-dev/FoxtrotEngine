#pragma once
#include "Actors/UIs/PanelUI.h"
#include "Components/Component.h"
#include "CommandHistory.h"
#include "FTCoreEditor.h"

class Scene;
class Actor;
class EditorLayer;

#define ACTORNAME_MAX 100

class EditorElement :
    public Actor
{
public:
	bool GetIsFocused() { return mIsFocused; }
	void SetIsFocused(bool isFocused) { mIsFocused = isFocused; }

public:
	// Runs on FTCoreEditor::UpdateGame() when mIsUpdatingGame is NOT true
	void EditorUpdate(float deltaTime);
	void EditorRender(FoxtrotRenderer* renderer);

public:
	void UpdateUI(bool isPremade);
	void RenderUI(FoxtrotRenderer* renderer);

public:
	// Constructors used for FTPremade origin, possibily to make a dummy
	// not recommended to use outside of FTPremade
	EditorElement();
	EditorElement(Actor* origin);

	// Use this when adding EditorElements to a scene.
	EditorElement(Scene* scene);
	EditorElement(Actor* actor, Scene* scene);
	EditorElement(EditorElement* origin, Scene* scene);

private:
	// Stores ActorGroup index -> is int for Uint -> int conversion not supported
	int		mActorGroupIdx;
	float	mRotationModSpeed;
	bool	mIsFocused;

private:
	void UpdateActorName();
	void UpdateActorGroup();
	void UpdateActorState();
	void UpdateActorWorldPosition();
	void UpdateActorLocalPosition();
	void UpdateActorScale();
	void UpdateActorRotation();

	void UpdateComponents();
	void DisplayCompSelectionPopup();

	void UpdateMakePrefabBtn();
};