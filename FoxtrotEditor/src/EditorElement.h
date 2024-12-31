// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// A holder object that has the values of an Actor.
/// This will be on Foxtrot Editor, and the user can modify its values.
/// Values of EditorElements will be saved as an ActorData in .Chunk file,
/// and will be used to create Actors in the produced game. 
/// </summary>

#pragma once
#include "Components/UIs/PanelUIComponent.h"
#include "Components/Component.h"
#include "CommandHistory.h"
#include "FTCoreEditor.h"

class Scene;
class Actor;
class EditorLayer;
class EditorScene;

#define ACTORNAME_MAX 100

class EditorElement :
    public Actor
{
public:
	bool GetIsFocused() { return mIsFocused; }
	void SetIsFocused(bool isFocused) { mIsFocused = isFocused; }

public:
	// Updates editor specific features -> this will be omitted from the produced game.
	void EditorUpdate(float deltaTime);

	// Renders editor specific features -> this will be omitted from the produced game.
	void EditorRender(FoxtrotRenderer* renderer);

public:
	/// <summary>
	/// Updates UIs displayed on Inspector menu.
	/// </summary>
	/// <param name="isPremade : ">Is this object premade?</param>
	void UpdateUI(bool isPremade);

public:
	/// <summary>
	/// Constructor that adds EditorElements to scene during initialization phase.
	/// (When a .chunk is being loaded)
	/// </summary>
	/// <param name="scene : "> A scene object which this EditorElement is loaded to.</param>
	EditorElement(EditorScene* scene);

	/// <summary>
	/// Constructor that is used for FTPremade UI.
	/// Fetches the values from FTPremade origin to modify them.
	/// Not recommended to use outside of FTPremade
	/// </summary>
	/// <param name="origin : "> Actor nested inside of Premade to copy values from. </param>
	EditorElement(Actor* actor);

	/// <summary>
	/// Constructor that is used for FTPremade origin.
	/// This Fetches the FTPremade origin, makes EditorElement and adds itself to EditorScene.
	/// Not recommended to use outside of FTPremade
	/// </summary>
	/// <param name="origin : "> Actor nested inside of Premade to copy values from. </param>
	EditorElement(Actor* actor, EditorScene* scene);

private:
	bool mIsFocused;  // Is this item clicked on Foxtrot Editor's hierarchy menu?

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

	void UpdateMakePrefabBtn();
};