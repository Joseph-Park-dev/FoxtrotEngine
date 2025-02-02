// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
///  for Rigidbody simulation based on Box2D.
/// </summary>

#pragma once
#include "Components/Component.h"
#include "Core/TemplateFunctions.h"

class FTVector2;
class Actor;

class Rigidbody2D : public Component
{
public:
	std::string  GetName() const override 
	{ 
		return "Rigidbody2D"; 
	}
	b2BodyId& GetBodyID();

public:
	virtual void Initialize	(FTCore* coreInstance)	override;
	virtual void LateUpdate	(float deltaTime)		override;

	virtual void CloneTo	(Actor* actor)          override;

public:
	Rigidbody2D(Actor* owner,
		int updateOrder = DEFAULT_UPDATEORDER);
	virtual ~Rigidbody2D() override;

private:
	b2BodyId	mBodyID;

public:
	virtual void LoadProperties(std::ifstream& ifs) override;

#ifdef FOXTROT_EDITOR
public:
	/* The engine expects no RigidBody attributes will be updated
	   during the runtime of the game.
	   Therefore, Saving the attributes will be only required on Editor.*/
	virtual void SaveProperties(std::ofstream& ofs) override;

public:
	virtual void EditorUpdate(float deltaTime)			 override;
	virtual void EditorRender(FoxtrotRenderer* renderer) override;

public:
	virtual void EditorUIUpdate() override;

private:
	// This is used only on editor to update the values.
	b2BodyDef mBodyDefCache;

private:
	void UpdateBodyType();
	void UpdateGravityScale();
#endif;
};