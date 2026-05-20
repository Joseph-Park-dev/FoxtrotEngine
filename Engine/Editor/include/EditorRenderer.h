#pragma once
#include "Renderer/IRenderer.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui/imgui.h>

namespace Core
{
	class IWindow;
}

namespace Editor
{
	class ViewportRenderer;
	class EditorRenderer :
		public Core::IRenderer
	{
	public:
		bool InitializeViewport(Core::IWindow* window, UINT posX, UINT posY, UINT width, UINT height);
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