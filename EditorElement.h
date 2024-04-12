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

			void Render(SDL_Renderer* renderer);
			void UIRender(SDL_Renderer* renderer);

public:
	EditorElement(Scene* scene);
	EditorElement(Actor* actor);
	~EditorElement();

private:
	// Stores ActorGroup index -> is int for Uint -> int conversion not supported
	int mActorGroupIdx;

private:
	void UpdateActorName();
	void UpdateActorGroup();
	void UpdateActorState();
	void UpdateActorWorldPosition();
	void UpdateActorLocalPosition();
	void UpdateActorScale();
	void UpdateActorRotation(TransSetFloatFunc func);
	void UpdateComponents();

	void DisplayCompSelectionPopup();
};

