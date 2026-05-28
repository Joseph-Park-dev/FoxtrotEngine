#pragma once
#include "Renderer/IRenderer.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui/imgui.h>

namespace Core
{
	class IWindow;
}

namespace D3D11
{
	class ViewportRenderer;
}

namespace Editor
{
	class EditorRenderer :
		public Core::IRenderer
	{
	public:
		bool InitializeViewport(Core::IWindow* window, UINT posX, UINT posY, UINT width, UINT height);
		void RenderOnViewport();
		void SetViewport(const ImVec2& topLeft, const ImVec2& resolution);

	public:
		D3D11::ViewportRenderer* GetViewportRenderer();

	public:
		EditorRenderer(Core::IWindow* window);

	private:
		D3D11::ViewportRenderer* mViewportRenderer;
	};
} // namespace Editor