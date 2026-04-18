#pragma once
#include "Renderer/D3D11Renderer.h"

#include <imgui/imgui.h>

namespace Editor
{
	class ViewportRenderer;
	class EditorRenderer :
		public D3D11::D3D11Renderer
	{
	public:
		bool InitializeViewport(Core::FTWindow* window, UINT posX, UINT posY, UINT width, UINT height);
		void RenderOnViewport();
		void SetViewport(const ImVec2& topLeft, const ImVec2& resolution);

	public:
		ViewportRenderer* GetViewportRenderer();

	public:
		EditorRenderer(D3D11::D3D11Window* window);

	private:
		ViewportRenderer* mViewportRenderer;
	};
} // namespace Editor