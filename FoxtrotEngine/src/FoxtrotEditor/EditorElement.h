#pragma once
#include "FoxtrotEngine/Actors/UIs/PanelUI.h"
#include "FoxtrotEditor/CommandHistory.h"

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
			void EditorUpdateComponents(float deltaTime);
			void EditorLateUpdateComponents(float deltaTime);
	virtual void EditorUpdateActor();
	virtual void EditorLateUpdateActor();
			void UpdateUI();

			void Render(FoxtrotRenderer* renderer);
			void UIRender(FoxtrotRenderer* renderer);

public:
	EditorElement(Scene* scene);
	EditorElement(Actor* actor);

private:
	// Stores ActorGroup index -> is int for Uint -> int conversion not supported
	int mActorGroupIdx;
	float mRotationModSpeed;

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