#pragma once
#include "Component.h"

#include "Core/TemplateFunctions.h"
#include "ResourceSystem/Mesh.h"

class FTVector2;

enum class CollidedSide
{
	NONE,
	LEFT,
	RIGHT,
	UP,
	DOWN
};

class ColliderComponent :
	public Component
{
public:
	virtual std::string GetName() const override
	{
		return "ColliderComponent";
	}
	virtual void LoadProperties(std::ifstream& ifs) override;

public:
	FTVector2 GetOffsetPos() const;
	// Get world position (anchor-pointed to the center)
	FTVector2	 GetFinalPosition() const;
	FTVector2	 GetScale() const;
	CollidedSide GetCollidedSide() const;
	uint32_t	 GetID() const;

	FTVector2& GetOffsetPosRef();
	FTVector2& GetScaleRef();

	b2Polygon& GetPolygon() { return mPolygon; }

	void SetOffsetPos(FTVector2 offsetPos);
	void SetScale(FTVector2 scale);

public:
	virtual void Initialize(FTCore* coreInstance) override;
	// This can also be used as refreshing method.
	void LateUpdate(float deltaTime) override;

public:
	ColliderComponent(class Actor* owner, int drawOrder, int updateOrder);
	ColliderComponent(const ColliderComponent& origin);
	//~ColliderComponent() override;
	virtual void CloneTo(Actor* actor) override;

public:
	// Shallow copying is not needed (duplicated ID)
	ColliderComponent& operator=(ColliderComponent& origin) = delete;
	friend class CollisionManager;
	friend class Actor;

private:
	void OnCollisionEnter(ColliderComponent* other);
	void OnCollisionStay(ColliderComponent* other);
	void OnCollisionExit(ColliderComponent* other);

	void OnRayEnter();

private:
	b2Polygon		mPolygon;

private:
	FTVector2		mOffsetPos;
	FTVector2		mFinalPosition;
	FTVector2		mScale;
	CollidedSide	mCollidedSide;

	static uint32_t g_nextID;
	uint32_t		mID;
	uint32_t		mColliCount;

#ifdef FOXTROT_EDITOR
public:
	virtual void SaveProperties(nlohmann::ordered_json& out) override;
	void		 Render(FoxtrotRenderer* renderer) override;
	void		 EditorUIUpdate() override;

private:
	void UpdateOffsetPos();
	void UpdateScale();
#endif
};