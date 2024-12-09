#pragma once
#include "Scenes/Scene.h"

#include <vector>

class EditorElement;

class EditorScene : public Scene
{
public:
	virtual void DeleteAll() override;

public:
	void UnfocusEditorElements();
	void AddEditorElement();
	void AddEditorElement(EditorElement* copied);
	// Deep copies the data from actor
	void AddEditorElement(Actor* actor);

public:
	void EditorUpdate(float deltaTime);
	void EditorRender(FoxtrotRenderer* renderer);

public:
	EditorScene();

private:
	bool mIsUpdatingActors;
};

