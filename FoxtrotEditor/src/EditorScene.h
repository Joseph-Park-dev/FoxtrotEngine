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

class EditorScene : public Scene
{
public:
	// Deletes all EditorElements & clearing the Scene.
	virtual void DeleteAll() override;

public:
	// Sets all EditorElements' focused status to false.
	void UnfocusEditorElements();

	// Adds an empty EditorElement.
	void AddEditorElement();

	/// <summary>
	/// Adds an EditorElement with copied values from actor.
	/// </summary>
	/// <param name="actor : ">Actor to copy values from.</param>
	void AddEditorElement(Actor* actor);

public:
	// Updates editor specific features -> this will be omitted from the produced game.
	void EditorUpdate(float deltaTime);

	// Renders editor specific features -> this will be omitted from the produced game.
	void EditorRender(FoxtrotRenderer* renderer);

public:
	EditorScene();

private:
	bool mIsUpdatingActors;
};

