// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Component for Rigidbody simulation based on Box2D.
/// </summary>

#pragma once
#include "Component/IComponent.h"

#include "box2d/box2d.h"

namespace Physics2D
{
	namespace ChunkKey
	{
		namespace Rigidbody2D
		{
			constexpr const char* NAME = "Rigidbody2D";

			constexpr const char* ALLOW_FAST_ROTATION = "Allow Fast Rotation";
			constexpr const char* ANGULAR_DAMPING	  = "Angular Damping";
			constexpr const char* ANGULAR_VELOCITY	  = "Angular Velocity";
			constexpr const char* ENABLE_SLEEP		  = "Enable Sleep";
			constexpr const char* FIXED_ROTATION	  = "Fixed Rotation";
			constexpr const char* GRAVITY_SCALE		  = "Gravity Scale";
			constexpr const char* IS_AWAKE			  = "Is Awake";
			constexpr const char* IS_BULLET			  = "Is Bullet";
			constexpr const char* IS_ENABLED		  = "Is Enabled";
			constexpr const char* INITIAL_VELOCITY	  = "Initial Velocity";
			constexpr const char* SLEEP_THRESHOLD	  = "Sleep Threshold";
			constexpr const char* BODY_TYPE			  = "Body Type";

			constexpr int BODY_TYPE_STATIC	  = 0;
			constexpr int BODY_TYPE_KINEMATIC = 1;
			constexpr int BODY_TYPE_DYNAMIC	  = 2;
		} // namespace Rigidbody2D
	} // namespace ChunkKey

	class Rigidbody2D : public Common::IComponent
	{
	public:
		const char* GetName() override
		{
			return "Rigidbody2D";
		}

		Common::IActor* GetOwner() override;
		b2BodyId&		GetBodyID();

	public:
		void Initialize() override;
		void Setup() override;
		void ProcessInput(InputSystem::IInputDevice* inputDevice) override;
		void Update(float deltaTime) override;
		void LateUpdate(float deltaTime) override;
		void Render(Graphics::IRenderer* renderer, Graphics::ICamera* camInst) override;

	public:
		explicit Rigidbody2D(Common::IActor* owner);
		~Rigidbody2D() override;

		void CloneTo(Common::IActor* actor) override;

	private:
		Common::IActor* mOwner;
		b2BodyId		mBodyID;

	public:
		void SaveProperties(std::ofstream& ofs) override;
		void LoadProperties(std::ifstream& ifs) override;

	private:
		void PrepareForSimulation();

#ifdef FOXTROT_EDITOR
	public:
		void EditorUpdate(float deltaTime) override;
		void EditorRender(Graphics::IRenderer* renderer, Graphics::ICamera* camInst) override;
		void EditorUIUpdate() override;

	private:
		b2BodyDef mBodyDefCache;

		void UpdateBodyType();
#endif
	};
} // namespace Physics2D
