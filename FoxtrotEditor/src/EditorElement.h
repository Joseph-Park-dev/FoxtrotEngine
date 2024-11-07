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
    public PanelUI
{
public:
	virtual void OnMouseLButtonClicked() override;

public:
			void CheckMouseHover() override;
			void OnMouseHovering() override;
			void OnMouseLButtonDown() override;

public:
			// Runs on FTCoreEditor::UpdateGame() when mIsUpdatingGame IS NOT true
			void EditorUpdate(float deltaTime);
			void EditorRender(FoxtrotRenderer* renderer);

public:
			void UpdateUI();
			void RenderUI(FoxtrotRenderer* renderer);

public:
	EditorElement(Scene* scene);
	EditorElement(Actor* actor, Scene* scene);

private:
	// Stores ActorGroup index -> is int for Uint -> int conversion not supported
	int		mActorGroupIdx;
	float	mRotationModSpeed;

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
};