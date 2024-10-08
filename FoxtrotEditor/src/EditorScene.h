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
	virtual void ProcessInput	(KeyInputManager* keyInputManager) override;
	virtual void Update			(float deltaTime) override;
	virtual void LateUpdate		(float deltaTime) override;
	virtual void Render			(FoxtrotRenderer* renderer) override;
	//virtual void ProcessEvent	() override;

public:
	void UnfocusEditorElements();
	void AddEditorElement();
	void AddEditorElement(Actor* actor);

public:
	EditorScene();

private:
	virtual void LoadData();
};

