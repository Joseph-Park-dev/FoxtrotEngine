#pragma once
#include <fstream>

#ifdef FOXTROT_EDITOR
#include "FTCoreEditor.h"
#endif

class Actor;
class FoxtrotRenderer;
class FTCore;
class KeyInputManager;

#define DEFAULT_DRAWORDER 100
#define DEFAULT_UPDATEORDER 100

#ifdef FOXTROT_EDITOR
#include <nlohmann/json.hpp>
#endif // FOXTROT_EDITOR

class Component
{
public:
	virtual std::string  GetName() const = 0;

public:
	virtual void Initialize(FTCore* coreInstance);
	virtual void ProcessInput(KeyInputManager* keyInputManager);
	virtual void Update(float deltaTime);
	virtual void LateUpdate(float deltaTime);
	virtual void Render(FoxtrotRenderer* renderer);

public:
	Actor*		GetOwner() const;
	const int	GetUpdateOrder() const;
	const int	GetDrawOrder() const;
	const bool	GetIsInitialized() const;

public:
	Component(class Actor* owner, int drawOrder = DEFAULT_DRAWORDER, int updateOrder = DEFAULT_UPDATEORDER);
	Component(const Component& origin);
	virtual ~Component();
	// Returns new object with copied values
	// (Needs to be distinguished with copy constructor)
	virtual void CloneTo(Actor* actor);

private:
	Actor*	mOwner;
	int		mUpdateOrder;
	int		mDrawOrder;
	
private:
	// This is turned as true as the Initialize(FTCore*) is executed.
	bool	mIsInitialized;

public:
	virtual void		 SaveProperties(std::ofstream& ofs);
	virtual void		 LoadProperties(std::ifstream& ifs);

public:
	template <class T>
	static void Load(Actor* actor, std::ifstream& ifs, FTCore* coreInst)
	{
		///Dynamically allocate actor of type T
		T* t = new T(actor, DEFAULT_DRAWORDER, DEFAULT_UPDATEORDER);
		// Load Properties first -> then initialize with the loaded values.
		t->LoadProperties(ifs);
	}

#ifdef FOXTROT_EDITOR
public:
	virtual void SaveProperties(nlohmann::ordered_json& out);
	virtual void LoadProperties(nlohmann::ordered_json& in);

public:
	// Member functions for EditorElement objects.
	// Functions for editor specific tasks, which means
	// codes to be executed in the Editor, but not in the produced game.
	// 
	// Renderer dependent components, 
	// (such as MeshRendererComponent, SpriteRendererComponent, etc.)
	// usually use FTCore level Update() & Render().
	virtual void EditorUpdate(float deltaTime) {};
	virtual void EditorRender(FoxtrotRenderer* renderer) {};

public:
	virtual void EditorUIUpdate();
	virtual void RenderDebugGeometries(FoxtrotRenderer* renderer, ImDrawList* imDrawList, FTVector2 screenCenter);

public:
	template <class T>
	static void Create(Actor* actor, FTCore* coreInst)
	{
		// Dynamically allocate actor of type T
		T* t = new T(actor, DEFAULT_DRAWORDER, DEFAULT_UPDATEORDER);
		// Call LoadProperties on new actor
		t->Initialize(coreInst);
	}
#endif // FOXTROT_EDITOR
};