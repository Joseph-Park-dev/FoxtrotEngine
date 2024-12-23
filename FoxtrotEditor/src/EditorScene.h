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
	void AddEditorElement(Actor* actor);
	//void AddEditorElement(EditorElement* copied);

public:
	void EditorUpdate(float deltaTime);
	void EditorRender(FoxtrotRenderer* renderer);

public:
	EditorScene();

private:
	bool mIsUpdatingActors;
};

