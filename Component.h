#pragma once
#include <fstream>

class FoxtrotRenderer;

#define DEFAULT_DRAWORDER 100
#define DEFAULT_UPDATEORDER 100

class Actor;

class Component
{
public:
	enum TypeID
	{
		AIComponent,
		AnimatorComponent,
		AnimSpriteComponent,
		BGSpriteComponent,
		ColliderComponent,
		InputMoveComponent,
		MoveComponent,
		Rigidbody2DComponent,
		SpriteComponent,
		TileMapComponent,

		NUM_COMPONENT_TYPES
	};

	static const char* TypeNames[NUM_COMPONENT_TYPES];

public:
	virtual void ProcessInput(class KeyInputManager* keyInputManager) {};
	virtual void Update		 (float deltaTime) {};
	virtual void LateUpdate	 (float deltaTime) {};
	virtual void Render		 (FoxtrotRenderer* renderer) {};

public:
	virtual std::wstring GetName()		  const = 0;
			Actor*		 GetOwner()		  const { return mOwner; }
	const	int			 GetUpdateOrder() const { return mUpdateOrder; }
	const	int			 GetDrawOrder()	  const { return mDrawOrder; }

private:
	Actor* mOwner;
	int	   mUpdateOrder;
	int	   mDrawOrder;

public:
	Component(class Actor* owner, int drawOrder = DEFAULT_DRAWORDER, int updateOrder = DEFAULT_UPDATEORDER);
	Component(const Component& origin);
	virtual ~Component();

	template<class T>
	static void Load(Actor* actor, std::ifstream& ifs)
	{
		// Dynamically allocate actor of type T
		T* t = new T(actor, DEFAULT_DRAWORDER, DEFAULT_UPDATEORDER);
		// Call LoadProperties on new actor
		t->LoadProperties(ifs);
	}

	template<class T>
	static void Create(Actor* actor)
	{
		// Dynamically allocate actor of type T
		new T(actor, DEFAULT_DRAWORDER, DEFAULT_UPDATEORDER);
	}
	
	// Returns new object with copied values 
	// (Needs to be distinguished with copy constructor)
	virtual void CloneTo(Actor* actor) {};

public:
	virtual void SaveProperties(std::ofstream& ofs);
	virtual void LoadProperties(std::ifstream& ifs);

public:
	virtual void EditorUpdate(float deltaTime){};
	virtual void EditorLateUpdate(float deltaTime) {};
	virtual void EditorRender(FoxtrotRenderer* renderer) {};
	virtual void EditorUIUpdate() {};
};