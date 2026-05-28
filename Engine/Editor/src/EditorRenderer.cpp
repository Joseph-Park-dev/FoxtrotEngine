#include "EditorRenderer.h"

#include "Renderer/ViewportRenderer.h"
#include "Debugging/DebugFuncs.h"
#include "Debugging/DebugMemAlloc.h"
#include "Plugin/GetFunc.h"
#include "Plugin/PluginKey.h"

namespace Editor
{
	bool EditorRenderer::InitializeViewport(Core::IWindow* window, UINT posX, UINT posY, UINT width, UINT height)
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

	D3D11::ViewportRenderer* EditorRenderer::GetViewportRenderer()
	{
		return mViewportRenderer;
	}

	EditorRenderer::EditorRenderer(Core::IWindow* window)
		: Core::IRenderer(window)
		, mViewportRenderer(nullptr)
	{
		using CREATE_VP_RENDERER = D3D11::ViewportRenderer* (*)();
		mViewportRenderer		 = GetFunc<CREATE_VP_RENDERER>(Plugin::Name::D3D11_EDITOR, ProcNames::D3D11::CREATE_VP_RENDERER)();
	}
} // namespace Editor