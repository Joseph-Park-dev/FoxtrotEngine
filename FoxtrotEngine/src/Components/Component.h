// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Base class for all Components.
/// </summary>

#pragma once
#include <fstream>

#include "Debugging/DebugMemAlloc.h"

#ifdef FOXTROT_EDITOR
#include "FTCoreEditor.h"
#endif

class Actor;
class FoxtrotRenderer;
class FTCore;
class KeyInputManager;
class Camera;

#define DEFAULT_DRAWORDER 100
#define DEFAULT_UPDATEORDER 100

class Component
{
#define CLONE_TO_NOT_IMPLEMENTED printf("ERROR: CloneTo of %s is not implemented.", typeid(this).name());

public:
	virtual std::string  GetName() const = 0;

public:
	virtual void Initialize		(FTCore* coreInstance);
	virtual void Setup			();
	virtual void ProcessInput	(KeyInputManager* keyInputManager);
	virtual void Update			(float deltaTime);
	virtual void LateUpdate		(float deltaTime);
	virtual void Render			(FoxtrotRenderer* renderer);

	virtual void CloneTo		(Actor* actor) = 0;

public:
	Actor*		GetOwner()			const;
	const int	GetUpdateOrder()	const;
	const int	GetDrawOrder()		const;
	const bool	GetIsInitialized()	const;
	const bool	GetIsSetup()		const;

public:
	/// <summary>
	/// Creates a Component to be assigned to the Actor.
	/// </summary>
	Component(
		Actor* owner, 
		int drawOrder = DEFAULT_DRAWORDER, 
		int updateOrder = DEFAULT_UPDATEORDER
	);
	
	/// <summary>
	/// Deep copies Component from another.
	/// </summary>
	/// <param name="comp : "> Component to copy values from.</param>
	Component(const Component* comp);
	virtual ~Component();

private:
	Actor*	mOwner;
	int		mUpdateOrder;
	int		mDrawOrder;
	
private:
	// This is turned as true as the Initialize(FTCore*) is executed.
	bool	mIsInitialized;
	// This is turned as true as the Setup() is executed.
	bool	mIsSetup;

public:
	virtual void SaveProperties(std::ofstream& ofs);
	virtual void LoadProperties(std::ifstream& ifs);

public:
	template <class T>
	static void Load(Actor* actor, std::ifstream& ifs)
	{
		///Dynamically allocate actor of type T
		T* t = DBG_NEW T(actor, DEFAULT_DRAWORDER, DEFAULT_UPDATEORDER);
		// Load Properties first -> then initialize with the loaded values.
		t->LoadProperties(ifs);
	}

#ifdef FOXTROT_EDITOR
public:
	// Member functions for EditorElement objects.
	// Functions for editor specific tasks, which means
	// codes to be executed in the Editor, but not in the produced game.
	// 
	// Renderer dependent components, 
	// (such as MeshRendererComponent, SpriteRendererComponent, etc.)
	// usually use FTCore Update() & Render().
	virtual void EditorUpdate(float deltaTime) {};
	virtual void EditorRender(FoxtrotRenderer* renderer) {};

public:
	virtual void EditorUIUpdate();
	virtual void UpdateDebugShapes(
		FTVector3 pos,
		FTVector3 rot,
		FTVector3 scale,
		Camera* cameraInst
	);

	void		 SetUpdateOrder(int updateOrder);

public:
	template <class T>
	static void Create(Actor* actor, FTCore* coreInst)
	{
		// Dynamically allocate actor of type T
		T* t = DBG_NEW T(actor, DEFAULT_DRAWORDER, DEFAULT_UPDATEORDER);
		// Call LoadProperties on DBG_NEW actor
		t->Initialize(coreInst);
	}

protected:
	virtual void OnConfirmUpdate();
#endif // FOXTROT_EDITOR
};