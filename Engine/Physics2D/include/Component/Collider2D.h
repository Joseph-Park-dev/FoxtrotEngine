// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Base class for all 2D Collider Components.
/// </summary>

#pragma once
#include "Component/IComponent.h"

#include "box2d/box2d.h"
#include "FTMath.h"

#include <unordered_map>

namespace Physics2D
{
	enum class CollisionState
	{
		Enter,
		Stay,
		Exit
	};

	using CollisionStatesMap = std::unordered_map<int32_t, CollisionState>;

	namespace ChunkKey
	{
		namespace Collider2D
		{
			constexpr const char* SHOW_DEBUG_SHAPE = "Show Debug Shape";
			constexpr const char* OFFSET		   = "Offset";
		} // namespace Collider2D
	} // namespace ChunkKey

	class Collider2D : public Common::IComponent
	{
	public:
		const char* GetName() override
		{
			return "Collider2D";
		}

		Common::IActor* GetOwner() override;

	public:
		b2ShapeId&			GetShapeID();
		Math::FTVector2		GetOffsetPos() const;
		Math::FTVector2&	GetOffsetPosRef();
		Math::FTVector2		GetFinalPosition() const;
		CollisionStatesMap& GetCollisionStates();

		void SetOffsetPos(Math::FTVector2 offsetPos);

	public:
		virtual void Initialize() override;
		void		 Setup() override;
		void		 ProcessInput(InputSystem::IInputDevice* inputDevice) override;
		void		 Update(float deltaTime) override;
		void		 LateUpdate(float deltaTime) override;
		virtual void Render(Graphics::IRenderer* renderer, Graphics::ICamera* camInst) override;

	public:
		explicit Collider2D(Common::IActor* owner);
		Collider2D(const Collider2D& origin);
		~Collider2D() override;

		void CloneTo(Common::IActor* actor) override;

	public:
		void SaveProperties(std::ofstream& ofs) override;
		void LoadProperties(std::ifstream& ifs) override;

	protected:
		bool IsShowingDebugShape();

		void SetShowDebugShape(bool isShowing);

#ifdef FOXTROT_EDITOR
	public:
		void EditorUpdate(float deltaTime) override;
		void EditorRender(Graphics::IRenderer* renderer, Graphics::ICamera* camInst) override;
		void EditorUIUpdate() override;

	protected:
		virtual void ToggleDebugShape();

	private:
		void UpdateOffsetPos();
#endif

	protected:
		Common::IActor* mOwner;

	private:
		b2ShapeId		   mShapeID;
		Math::FTVector2	   mOffset;
		Math::FTVector2	   mFinalPosition;
		CollisionStatesMap mCollisionStates;

#ifdef FOXTROT_EDITOR
		bool mShowDebugShape;
#endif // FOXTROT_EDITOR
	};
} // namespace Physics2D
