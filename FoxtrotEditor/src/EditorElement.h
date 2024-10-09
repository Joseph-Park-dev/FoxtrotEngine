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
	template <class T>
	static void Create(Actor* actor)
	{
		// Dynamically allocate actor of type T
		T* t = new T(actor, DEFAULT_DRAWORDER, DEFAULT_UPDATEORDER);
		// Call LoadProperties on new actor
		t->Initialize(FTCoreEditor::GetInstance());
	}

public:
			void UpdateUI();
			void RenderUI(FoxtrotRenderer* renderer);

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