// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Main renderer from FoxtrotEngine.
/// Takes care of the rendering operations in DirectX 11 context.
/// </summary>

#pragma once
namespace Math
{
	class FTVector2;
}

namespace Core
{
	class Transform;
	class ViewportRenderer;
	class FTWindow;
	class FoxtrotRenderer
	{
	public:
		FoxtrotRenderer() {};

	protected:
		virtual bool Initialize(FTWindow* window) = 0;

#ifdef FOXTROT_EDITOR
	public:
		bool InitializeViewport(FTWindow* window, UINT posX, UINT posY, UINT width, UINT height);
		void RenderOnViewport();
		void SetViewport(const ImVec2& topLeft, const ImVec2& resolution);

	public:
		ViewportRenderer* GetViewportRenderer();

	private:
		ViewportRenderer* mViewportRenderer;
#endif // FOXTROT_EDITOR
	};
} // namespace Core