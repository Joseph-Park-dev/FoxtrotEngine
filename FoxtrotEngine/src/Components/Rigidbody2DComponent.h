#pragma once
#include "Components/Component.h"
#include "Core/TemplateFunctions.h"

class FTVector2;
class Actor;

class Rigidbody2DComponent : public Component
{
public:
	void AddForce(FTVector2 force);
	void AddVelocity(FTVector2 velocity);

	virtual void LoadProperties(std::ifstream& ifs) override;

public:
	float		 GetMass() { return mMass; }
	std::string  GetName() const override { return "Rigidbody2DComponent"; }
	bool		 GetIsGrounded() { return mIsGrounded; }
	bool		 GetIsBlockedUp() { return mIsBlockedUp; }

	void SetMass(float mass) { mMass = mass; }
	void SetVelocity(FTVector2 velocity) { mVelocity = velocity; }
	void SetMaxVelocity(FTVector2 velocity) { mMaxVelocity = velocity; }
	void SetIsGrounded(bool val) { mIsGrounded = val; }
	void SetIsBlockedUp(bool val) { mIsBlockedUp = val; }
	void SetAccelAlpha(FTVector2 accel) { mAccelerationA = accel; }

public:
	virtual void Initialize(FTCore* coreInstance) override{};
	virtual void Update(float deltaTime) override{};
	virtual void LateUpdate(float deltaTime) override;

public:
	Rigidbody2DComponent(Actor* owner, int drawOrder = DEFAULT_DRAWORDER,
		int updateOrder = DEFAULT_UPDATEORDER);
	~Rigidbody2DComponent(){};
	virtual void CloneTo(Actor* actor) override;

private:
	FTVector2 mForce;
	FTVector2 mAcceleration;
	FTVector2 mAccelerationA;
	float	  mMass;

	FTVector2 mVelocity;
	FTVector2 mMaxVelocity;
	float	  mFrictionCoeff;
	bool	  mIsGrounded;
	bool	  mIsBlockedUp;

	// Box2D attributes
	// b2Body*     mBody;

private:
	void UpdateLinearPosition(float deltaTime);
	void UpdateAcceleration(float deltaTime);
	void UpdateGravity(float deltaTime);
	void UpdateVelocity(float deltaTime);
	void ClearForceAndAccel();

#ifdef FOXTROT_EDITOR
public:
	virtual void SaveProperties(nlohmann::ordered_json& out) override;
#endif;
};