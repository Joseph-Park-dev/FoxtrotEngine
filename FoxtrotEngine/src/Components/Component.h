#pragma once
#include <fstream>
#include <nlohmann/json.hpp>

#include "EditorLayer.h"

#define DEFAULT_DRAWORDER 100
#define DEFAULT_UPDATEORDER 100

class Actor;
class FoxtrotRenderer;
class FTCore;
class KeyInputManager;

class Component
{
public:
	virtual std::wstring GetName() const = 0;
	virtual void		 LoadProperties(std::ifstream& ifs);

public:
	template <class T>
	static void Create(Actor* actor)
	{
		// Dynamically allocate actor of type T
		T* t = new T(actor, DEFAULT_DRAWORDER, DEFAULT_UPDATEORDER);
		// Call LoadProperties on new actor
		t->Initialize(FTCoreEditor::GetInstance());
	}

	template <class T>
	static void Load(Actor* actor, std::ifstream& ifs)
	{
		// Dynamically allocate actor of type T
		// T* t = new T(actor, DEFAULT_DRAWORDER, DEFAULT_UPDATEORDER);
		// Call LoadProperties on new actor
		// t->LoadProperties(ifs);
	}

public:
	virtual void Initialize(FTCore* coreInstance) = 0;
	virtual void ProcessInput(KeyInputManager* keyInputManager);
	virtual void Update(float deltaTime);
	virtual void LateUpdate(float deltaTime);
	virtual void Render(FoxtrotRenderer* renderer);

public:
	Actor*	  GetOwner() const;
	const int GetUpdateOrder() const;
	const int GetDrawOrder() const;

public:
	Component(class Actor* owner, int drawOrder = DEFAULT_DRAWORDER, int updateOrder = DEFAULT_UPDATEORDER);
	Component(const Component& origin);
	virtual ~Component();
	// Returns new object with copied values
	// (Needs to be distinguished with copy constructor)
	virtual void CloneTo(Actor* actor);

private:
	Actor* mOwner;
	int	   mUpdateOrder;
	int	   mDrawOrder;

#ifdef FOXTROT_EDITOR
public:
	virtual void SaveProperties(nlohmann::ordered_json& out);

public:
	virtual void EditorUpdate(float deltaTime);
	virtual void EditorLateUpdate(float deltaTime);
	virtual void EditorRender(FoxtrotRenderer* renderer);
	virtual void EditorUIUpdate();
#endif // FOXTROT_EDITOR
};