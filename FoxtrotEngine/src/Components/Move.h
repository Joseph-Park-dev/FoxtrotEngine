// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Moves the character by changing the Rigidbody's attributes,
/// i.e. setting velocity and giving force to the body.
/// </summary>

#pragma once
#include "Components/Component.h"

#include "Core/TemplateFunctions.h"

class FTVector2;
class Rigidbody2D;
struct Steering;
struct b2Vec2;
struct b2QueryFilter;

enum Controllable
{
	NO,
	YES
};

class Move :
	public Component
{
public:
	virtual FTDS::String GetName() const override
	{
		return "Move";
	}

public:
	float		 GetForwardSpeed() const { return mForwardSpeed; }
	float		 GetAngularSpeed() const { return mAngularSpeed; }
	float		 GetJumpForce() const { return mJumpForce; }
	bool		 GetIsGrounded() const { return mIsGrounded; }
	Controllable IsControllable() const { return mIsControllable; }

	void SetForwardSpeed(float speed) { mForwardSpeed = speed; }
	void SetAngularSpeed(float speed) { mAngularSpeed = speed; }
	void SetJumpForce(float jumpForce) { mJumpForce = jumpForce; }

public:
	virtual void Initialize(FTCore* coreInstance) override;
	virtual void LateUpdate(float deltaTime) override;

	virtual void CloneTo(Actor* actor) override;

public:
	Move(class Actor* owner, int updateorder);

protected:
	void Accelerate(b2Vec2 currVel, const Steering* steering);
	void Jump(b2Vec2 currVel, const Steering* steering);
	void SetIsGrounded();

private:
	Rigidbody2D*  mRigidbody;
	b2QueryFilter mGroundFilter;

	float mForwardSpeed;
	float mAngularSpeed;
	float mJumpForce;

	Controllable mIsControllable;
	bool		 mIsGrounded;

public:
	virtual void SaveProperties(std::ofstream& ofs) override;
	virtual void LoadProperties(std::ifstream& ifs) override;

#ifdef FOXTROT_EDITOR
	virtual void EditorUIUpdate() override;
#endif // FOXTROT_EDITOR
};

namespace ChunkKey
{
	constexpr const char* FORWARD_SPEED = "Forward Speed";
	constexpr const char* JUMP_FORCE	= "Jump Force";
	constexpr const char* ANGULAR_SPEED = "Angular Speed";
} // namespace ChunkKey