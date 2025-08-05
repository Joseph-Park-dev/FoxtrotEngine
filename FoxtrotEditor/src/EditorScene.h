// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// An object that emulates the game scene.
/// This shows how the Actors are arranged & interacts in a Scene
/// in the produced game.
/// </summary>

#pragma once
#include "Scenes/Scene.h"

#include <vector>

class EditorElement;

class EditorScene
{
public:
	// Sets all EditorElements' focused status to false.
	void UnfocusEditorElements();

	// Adds an empty EditorElement.
	void AddEditorElement();

	/// <summary>
	/// Adds an EditorElement with copied values from actor.
	/// Useful when Stopping a scene on FTEditor.
	/// </summary>
	/// <param name="actor : ">Actor to copy values from.</param>
	void AddEditorElement(Actor* actor);
	void AddEditorElement(EditorElement* element);

	EditorElement* FindEditorElement(FTDS::String& name, Actor* filter);
	EditorElement* FindEditorElement(const char* name, Actor* filter);

	void RemoveEditorElement(EditorElement* element);

public:
	std::vector<EditorElement*>& GetEditorElements();

public:
	void Initialize(FTCore* coreInst);
	void Setup();

	void ProcessInput(class FTInputDevice* inputDevice);
	// Runs on editor when pressing Play button.
	void Update(float deltaTime);
	void LateUpdate(float deltaTime);
	// Runs on editor when pressing Play button.
	void Render(FoxtrotRenderer* renderer);

	// Deletes all EditorElements & clearing the Scene.
	void DeleteAll();

public:
	// Updates editor specific features -> this will be omitted from the produced game.
	void EditorUpdate(float deltaTime);

	// Renders editor specific features -> this will be omitted from the produced game.
	void EditorRender(FoxtrotRenderer* renderer);

public:
	EditorScene();
	~EditorScene();

private:
	std::vector<EditorElement*> mEditorElements;
	bool						mIsUpdatingActors;
};
