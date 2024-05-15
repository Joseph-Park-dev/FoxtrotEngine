#pragma once
#include "PanelUI.h"
#include "CommandHistory.h"

class Scene;
class Actor;
class EditorLayer;

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
			void UIUpdate();

			void Render(FoxtrotRenderer* renderer);
			void UIRender(FoxtrotRenderer* renderer);

public:
	EditorElement(Scene* scene);
	EditorElement(Actor* actor);
	virtual ~EditorElement() override;

private:
	// Stores ActorGroup index -> is int for Uint -> int conversion not supported
	int mActorGroupIdx;
	float mRotationModSpeed;

private:
	void UpdateActorName();
	void UpdateActorGroup();
	void UpdateActorState();
	void UpdateActorWorldPosition(float modSpeed);
	void UpdateActorLocalPosition(float modSpeed);
	void UpdateActorScale(float modSpeed);
	void UpdateActorRotation(TransSetFloatFunc func);
	void UpdateComponents();

	void DisplayCompSelectionPopup();
};

