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
	// Runs on FTCoreEditor::UpdateGame() when mIsUpdatingGame is NOT true
	void EditorUpdate(float deltaTime);
	void EditorRender(FoxtrotRenderer* renderer);

public:
	void UpdateUI(bool isPremade);
	void RenderUI(FoxtrotRenderer* renderer);

public:
	/// <summary>
	/// Constructor that creates the empty EditorElement to be used in FTEditor.
	/// </summary>
	//EditorElement();

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

	/// <summary>
	/// Constructor that deep-copies the EditorElement, and adds itself to EditorScene.
	/// Especially useful to duplicate EditorElements on FTEditor.
	/// </summary>
	/// <param name="origin : ">EditorElement to be copied.</param>
	/// <param name="scene : ">EditorScene this object will be added.</param>
	EditorElement(EditorElement* origin, EditorScene* scene);

	/*
	/// <summary>
	/// Constructor called when "Stop" is triggered in FTEditor.
	/// Trans
	/// </summary>
	/// <param name="actor : "> Actor in scene whose values are copied from.</param>
	/// <param name="scene : "> A scene object which this EditorElement is loaded to.</param>
	EditorElement(Actor* actor, EditorScene* scene);
	EditorElement(EditorElement* origin, EditorScene* scene);
	*/

private:
	float	mRotationModSpeed;
	bool	mIsFocused;

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