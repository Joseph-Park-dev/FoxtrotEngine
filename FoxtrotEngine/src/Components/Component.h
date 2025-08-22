// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Base class for all Components.
///
/// [Design philosophy]
/// The Component controls its elements in an upper level,
/// and the element has the related operation.
///
/// Means of access to the elements (Getters & Setters) are not to be
/// exposed. Make them protected, or minimize the cases of them being public.
/// </summary>

#pragma once
#include <fstream>

#include "Debugging/DebugMemAlloc.h"

class Actor;
class FoxtrotRenderer;
class FTCore;
class FTInputDevice;
class Camera;
class FTVector3;

namespace DefaultVal
{
	constexpr int UPDATE_ORDER = 100;
}

namespace FTDS
{
	class String;
}

class Component
{
#define CLONE_TO_NOT_IMPLEMENTED printf("ERROR: CloneTo of %s is not implemented.", typeid(this).name());

public:
	virtual FTDS::String GetName() const = 0;

public:
	virtual void Initialize(FTCore* coreInstance);
	virtual void Setup();
	virtual void ProcessInput(FTInputDevice* inputDevice);
	virtual void Update(float deltaTime);
	virtual void LateUpdate(float deltaTime);
	virtual void Render(FoxtrotRenderer* renderer);

	virtual void CloneTo(Actor* actor) = 0;

public:
	Actor*	   GetOwner() const;
	const int  GetUpdateOrder() const;
	const bool GetIsInitialized() const;
	const bool GetIsSetup() const;
	const bool GetIsActive() const;

	void SetIsActive(bool isActive);

public:
	/// <summary>
	/// Creates a Component to be assigned to the Actor.
	/// </summary>
	Component(
		Actor* owner,
		int	   updateOrder = DefaultVal::UPDATE_ORDER);

	/// <summary>
	/// Deep copies Component from another.
	/// </summary>
	/// <param name="comp : "> Component to copy values from.</param>
	Component(const Component* comp);
	virtual ~Component();

private:
	Actor* mOwner;
	int	   mUpdateOrder;

private:
	// This is turned as true as the Initialize(FTCore*) is executed.
	bool mIsInitialized;
	// This is turned as true as the Setup() is executed.
	bool mIsSetup;
	// The component is alive in the game loop
	// (During ProcessInput, Update, LateUpdate and Render)
	bool mIsActive;

public:
	virtual void SaveProperties(std::ofstream& ofs);
	virtual void LoadProperties(std::ifstream& ifs);

public:
	template <class T>
	static void Load(Actor* actor, std::ifstream& ifs)
	{
		/// Dynamically allocate actor of type T
		T* t = DBG_NEW T(actor, DefaultVal::UPDATE_ORDER);
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

	void SetUpdateOrder(int updateOrder);

public:
	template <class T>
	static void Create(Actor* actor, FTCore* coreInst)
	{
		// Dynamically allocate actor of type T
		T* t = DBG_NEW T(actor, DefaultVal::UPDATE_ORDER);
		// Call LoadProperties on DBG_NEW actor
		t->Initialize(coreInst);
	}

#endif // FOXTROT_EDITOR
};

namespace ChunkKey
{
	constexpr const char* COMPONENTS   = "Components";
	constexpr const char* UPDATE_ORDER = "UpdateOrder";
	constexpr const char* IS_ACTIVE	   = "Is Active";
} // namespace ChunkKey