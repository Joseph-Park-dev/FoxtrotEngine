// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Base class for all 2D Collider s.
/// </summary>

#pragma once
#include "Components/Component.h"

#include "box2d/box2d.h"

#include "Core/TemplateFunctions.h"
#include "ResourceSystem/Mesh.h"

#ifdef FOXTROT_EDITOR
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#endif // FOXTROT_EDITOR

class FTVector2;
class Actor;
enum class CollisionState;

using CollisionStatesMap = std::unordered_map<int32_t, CollisionState>;

class Collider2D :
	public Component
{
public:
	virtual std::string GetName() const override
	{
		return "Collider2D";
	}

public:
	b2ShapeId&				GetShapeID();
	FTVector2				GetOffsetPos()			const;
	FTVector2&				GetOffsetPosRef();
	// Get world position of the collider 
	// (anchor-pointed to the center)
	FTVector2				GetFinalPosition()		const;
	CollisionStatesMap&		GetCollisionStates();

	void SetOffsetPos(FTVector2 offsetPos);

public:
	// This can also be used when refreshing the object.
	virtual void Initialize	(FTCore* coreInstance)	override;
			void LateUpdate	(float deltaTime)		override;
			
	virtual void CloneTo	(Actor* actor)			override;

public:
	Collider2D(Actor* owner, int updateOrder);
	Collider2D(const Collider2D& origin);
	virtual ~Collider2D() override;

public:
	//Collider2D& operator=(Collider2D& origin);
	friend class CollisionManager;
	friend class Actor;

private:
	void OnCollisionEnter(Collider2D* other);
	void OnCollisionStay(Collider2D* other);
	void OnCollisionExit(Collider2D* other);

	//void OnRayEnter();

private:
	b2ShapeId				mShapeID;
	FTVector2				mOffset;
	FTVector2				mFinalPosition;

	CollisionStatesMap		mCollisionStates;

public:
	virtual void SaveProperties(std::ofstream& ofs) override;
	virtual void LoadProperties(std::ifstream& ifs) override;

#ifdef FOXTROT_EDITOR
public:
	virtual void EditorUpdate(float deltaTime)			 override;
	virtual void EditorRender(FoxtrotRenderer* renderer) override;

public:
	virtual	void EditorUIUpdate() override;

protected:
	virtual void ToggleDebugShape();
			bool IsShowingDebugShape();

private:
	bool mShowDebugShape;

private:
	void UpdateOffsetPos();
#endif
};