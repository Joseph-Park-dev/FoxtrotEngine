#pragma once
#include "Component.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>

#include "Core/TemplateFunctions.h"
#include "ResourceSystem/Mesh.h"

class FTVector2;

class Collider2DComponent :
	public Component
{
public:
	virtual std::string GetName() const override
	{
		return "Collider2DComponent";
	}

public:
	void CreateShape(b2BodyId bodyID, b2ShapeDef* shapeDef, b2Polygon* polygon);

public:
	// Use Setters to assign data out of this class.
	// But I allowed this can be modified exceptionally 
	// to prevent pass-by-value.
	b2Polygon&	GetPolygonRef() { return mPolygon; }

	FTVector2	GetOffsetPos() const;
	// Get world position of the collider 
	// (anchor-pointed to the center)
	FTVector2	GetFinalPosition() const;

	FTVector2&	GetOffsetPosRef();

	void		SetOffsetPos(FTVector2 offsetPos);

public:
	// This can also be used when refreshing the object.
	virtual void Initialize(FTCore* coreInstance) override;
			void LateUpdate(float deltaTime) override;

public:
	Collider2DComponent(class Actor* owner, int drawOrder, int updateOrder);
	Collider2DComponent(const Collider2DComponent& origin);
	virtual ~Collider2DComponent() override;
	//virtual void CloneTo(Actor* actor) override;

public:
	// Shallow copying is not needed (duplicated ID)
	Collider2DComponent& operator=(Collider2DComponent& origin) = delete;
	friend class CollisionManager;
	friend class Actor;

private:
	void OnCollisionEnter(Collider2DComponent* other);
	void OnCollisionStay(Collider2DComponent* other);
	void OnCollisionExit(Collider2DComponent* other);

	void OnRayEnter();

private:
	b2Polygon		mPolygon;

	FTVector2		mOffset;
	FTVector2		mFinalPosition;

public:
	virtual void SaveProperties(std::ofstream& ofs) override;
	virtual void LoadProperties(std::ifstream& ifs) override;

#ifdef FOXTROT_EDITOR
public:
	virtual void SaveProperties(nlohmann::ordered_json& out) override;

public:
	virtual void EditorUpdate(float deltaTime)			 override;
	virtual void EditorRender(FoxtrotRenderer* renderer) override;

public:
	virtual	void EditorUIUpdate() override;
	virtual	void RenderDebugGeometries(
				ImDrawList* imDrawList, 
				FTVector2 screenCenter
			) = 0;

private:
	void UpdateOffsetPos();
#endif
};