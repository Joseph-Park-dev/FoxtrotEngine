#pragma once
#include "Scenes/Scene.h"

#include <vector>

class EditorScene : public Scene
{
public:
	virtual void Enter() override;
	virtual void Exit()  override;

	virtual void DeleteAll() override;

public:
	void UnfocusEditorElements();
	void AddEditorElement();
	void AddEditorElement(Actor* actor);

public:
	void EditorUpdate(float deltaTime);
	void EditorRender(FoxtrotRenderer* renderer);

public:
	EditorScene();

private:
	bool mIsUpdatingActors;

private:
	virtual void LoadData();
};

