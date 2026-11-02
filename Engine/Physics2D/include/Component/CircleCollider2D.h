// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Collider shaped as a circle.
/// </summary>

#pragma once
#include "Component/Collider2D.h"

namespace Physics2D
{
	namespace ChunkKey
	{
		namespace CircleCollider2D
		{
			constexpr const char* NAME = "CircleCollider2D";
		}
	} // namespace ChunkKey

	class CircleCollider2D : public Collider2D
	{
	public:
		const char* GetName() override
		{
			return "CircleCollider2D";
		}

		const float GetRadius() const;
		void		SetRadius(float radius);

	public:
		void Initialize() override;

	public:
		explicit CircleCollider2D(Common::IActor* owner);
		~CircleCollider2D() override;

		void CloneTo(Common::IActor* actor) override;

	private:
		Math::FTVector2 mCenter;
		float			mRadius;

	public:
		void SaveProperties(std::ofstream& ofs) override;
		void LoadProperties(std::ifstream& ifs) override;

#ifdef FOXTROT_EDITOR
	public:
		void EditorUpdate(float deltaTime) override;
		void EditorRender(Graphics::IRenderer* renderer, Graphics::ICamera* camInst) override;
		void EditorUIUpdate() override;
#endif
	};
} // namespace Physics2D
