#pragma once
#include "Components/Component.h"
#include "Core/TemplateFunctions.h"

class FTVector2;
class Actor;

class Rigidbody2DComponent : public Component
{
public:
	std::string  GetName() const override 
	{ 
		return "Rigidbody2DComponent"; 
	}
	b2BodyId& GetBodyID();

public:
	virtual void Initialize(FTCore* coreInstance) override;
	virtual void Update(float deltaTime) override{};
	virtual void LateUpdate(float deltaTime) override;

public:
	Rigidbody2DComponent(Actor* owner, int drawOrder = DEFAULT_DRAWORDER,
		int updateOrder = DEFAULT_UPDATEORDER);
	virtual ~Rigidbody2DComponent() override;

private:
	b2BodyId	mBodyID;

public:
	virtual void LoadProperties(std::ifstream& ifs) override;

#ifdef FOXTROT_EDITOR
public:
	/* The engine expects no RigidBody attributes will be updated
	   during the runtime of the game.
	   Therefore, Saving the attributes will only be required on Editor.*/
	virtual void SaveProperties(std::ofstream& ofs) override;

public:
	virtual void EditorUpdate(float deltaTime)			 override;
	virtual void EditorRender(FoxtrotRenderer* renderer) override;

public:
	virtual void EditorUIUpdate() override;
	virtual void CloneTo(Actor* actor) override;

private:
	// This is used only on editor to update the values.
	b2BodyDef mBodyDefCache;

private:
	void UpdateBodyType();
	void UpdateGravityScale();
#endif;
};