#include "EditorRenderer.h"

#include "ViewportRenderer.h"
#include "Debugging/DebugFuncs.h"
#include "Debugging/DebugMemAlloc.h"

namespace Editor
{
	bool EditorRenderer::InitializeViewport(Core::FTWindow* window, UINT posX, UINT posY, UINT width, UINT height)
	{
		if (!mViewportRenderer)
		{
			Debug::LogError(__LINE__, __FILE__, "CreateRenderTexture failed.");
			return false;
		}
		ImVec2 pos;
		pos.x = static_cast<float>(posX);
		pos.y = static_cast<float>(posY);

		ImVec2 size;
		size.x = static_cast<float>(width);
		size.y = static_cast<float>(height);
		mViewportRenderer->InitializeTexture(this, size);
		return true;
	}

	void EditorRenderer::RenderOnViewport()
	{
		mViewportRenderer->DrawOnTexture(this);
	}

	void EditorRenderer::SetViewport(const ImVec2& topLeft, const ImVec2& resolution)
	{
		// Set the viewport
		ZeroMemory(GetViewport(), sizeof(D3D11_VIEWPORT));

		GetViewport()->TopLeftX = topLeft.x;
		GetViewport()->TopLeftY = topLeft.y;
		GetViewport()->Width	= resolution.x;
		GetViewport()->Height	= resolution.y;
		// m_screenViewport.Width = static_cast<float>(m_screenHeight);
		GetViewport()->MinDepth = 0.0f;
		GetViewport()->MaxDepth = 1.0f; // Note: important for depth buffering
		GetContext()->RSSetViewports(1, GetViewport());
	}

	ViewportRenderer* EditorRenderer::GetViewportRenderer()
	{
		return mViewportRenderer;
	}

	EditorRenderer::EditorRenderer(D3D11::D3D11Window* window)
		: D3D11::D3D11Renderer(window)
		, mViewportRenderer(DBG_NEW ViewportRenderer)
	{
	}
} // namespace Editor