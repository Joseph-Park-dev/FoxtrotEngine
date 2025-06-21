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
#include "Components/UIs/PanelUI.h"
#include "Components/Component.h"
#include "CommandHistory.h"
#include "FTCoreEditor.h"

class Scene;
class Actor;
class EditorLayer;
class EditorScene;
class FTPremade;

#define ACTORNAME_MAX 100

class EditorElement :
	public Actor
{
public:
	const bool	 GetIsFocused() const;
	const size_t GetHierarchyLevel() const;
	const bool GetIsDisplayed() const;

	void SetIsFocused(bool isFocused);
	void SetHierarchyLevel(size_t lv);
	void SetIsDisplayed(bool isDisplayed);

public:
	void Initialize(FTCore* coreInst) override;
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
	/// Constructor that creates empty EditorElement.
	/// It should be added to the EditorScene manually.
	EditorElement();

	/// <summary>
	/// Constructor that deep-copies an Actor object.
	/// </summary>
	/// <param name="origin : ">An Actor object to deep-copy values from.</param>
	EditorElement(Actor* actor);

	/// <summary>
	/// Constructor that is used for FTPremade origin.
	/// This Fetches the FTPremade origin, makes EditorElement.
	/// Not recommended to use outside of FTPremade
	EditorElement(FTPremade* premade);

private:
	bool   mIsFocused;		// Is this item clicked on Foxtrot Editor's Hierarchy menu?
	size_t mHierarchyLevel; // How many parent Actors are there for this object?

	bool mIsDisplayed;  // Is this item displayed on Hierarchy menu during this frame?

private:
	void UpdateActorName();
	void UpdateActorGroup();
	void UpdateDrawOrder();
	void UpdateActorState();

	void UpdateComponentsUI();
	void DisplayCompSelectionPopup();

	void UpdateMakePrefabBtn();
};