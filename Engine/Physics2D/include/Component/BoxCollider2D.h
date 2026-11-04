// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Collider shaped as a box, a square, or a rectangle.
/// </summary>

#pragma once
#include "Component/Collider2D.h"

#ifdef FOXTROT_EDITOR
namespace D3D11
{
	class FTRectangle;
}
#endif // FOXTROT_EDITOR

namespace Physics2D
{
	namespace ChunkKey
	{
		namespace BoxCollider2D
		{
			constexpr const char* NAME			 = "BoxCollider2D";
			constexpr const char* COLLIDER_SCALE = "Collider Scale";
		} // namespace BoxCollider2D
	} // namespace ChunkKey

	class BoxCollider2D : public Collider2D
	{
	public:
		const char* GetName() override
		{
			return "BoxCollider2D";
		}

		const Math::FTVector2 GetScale() const;
		void				  SetScale(Math::FTVector2 scale);

	public:
		void Initialize() override;
		void Render(Graphics::IRenderer* renderer, Graphics::ICamera* camInst) override;

	public:
		explicit BoxCollider2D(Common::IActor* owner);
		~BoxCollider2D() override;

		void CloneTo(Common::IActor* actor) override;

	private:
		Math::FTVector2 mScale;

	public:
		void SaveProperties(std::ofstream& ofs) override;
		void LoadProperties(std::ifstream& ifs) override;

	private:
		bool RigidBodyExists();

		/// <summary>
		/// Box centered on the collider offset, sized by the actor scale and collider scale.
		/// </summary>
		b2Polygon MakePolygon();
		void	 ApplyPolygon();

#ifdef FOXTROT_EDITOR
	public:
		void EditorUpdate(float deltaTime) override;
		void EditorRender(Graphics::IRenderer* renderer, Graphics::ICamera* camInst) override;
		void EditorUIUpdate() override;

		void UpdateDebugShapes(Math::FTVector3 pos, Math::FTVector3 rot, Math::FTVector3 scale, Graphics::ICamera* cameraInst);

	private:
		void ToggleDebugShape() override;
		void UpdateScale();

		/// <summary>
		/// Creates the debug rectangle on the device, then places it for the camera about to draw it.
		/// </summary>
		void SyncDebugRect(Graphics::IRenderer* renderer, Graphics::ICamera* camInst);

		/// <summary>
		/// Editor viewport rectangle for this box. DebugShapes owns and deletes it after AddShape.
		/// </summary>
		D3D11::FTRectangle* mDebugRect;
		/// <summary>
		/// Set when GPU setup for the debug rectangle fails, so the editor does not retry every frame.
		/// </summary>
		bool mDebugGpuFailed;
#endif
	};
} // namespace Physics2D
