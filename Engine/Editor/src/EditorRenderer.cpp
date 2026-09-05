#include "EditorRenderer.h"

#include "Renderer/ViewportRenderer.h"
#include "Debugging/DebugFuncs.h"
#include "Debugging/DebugMemAlloc.h"
#include "Plugin/GetFunc.h"
#include "Plugin/PluginKey.h"

//namespace Editor
//{
//	void EditorRenderer::SetViewport(const ImVec2& topLeft, const ImVec2& resolution)
//	{
//		// Set the viewport
//		ZeroMemory(GetViewport(), sizeof(D3D11_VIEWPORT));
//
//		GetViewport()->TopLeftX = topLeft.x;
//		GetViewport()->TopLeftY = topLeft.y;
//		GetViewport()->Width	= resolution.x;
//		GetViewport()->Height	= resolution.y;
//		// m_screenViewport.Width = static_cast<float>(m_screenHeight);
//		GetViewport()->MinDepth = 0.0f;
//		GetViewport()->MaxDepth = 1.0f; // Note: important for depth buffering
//		GetContext()->RSSetViewports(1, GetViewport());
//	}
//
//	D3D11::ViewportRenderer* EditorRenderer::GetViewportRenderer()
//	{
//		return mViewportRenderer;
//	}
//
//	EditorRenderer::EditorRenderer()
//		: mViewportRenderer(nullptr)
//	{
//		using CREATE_VP_RENDERER = D3D11::ViewportRenderer* (*)();
//		mViewportRenderer		 = Core::GetFunc<CREATE_VP_RENDERER>(Core::Plugin::Name::D3D11_EDITOR, Core::ProcNames::D3D11::CREATE_VP_RENDERER)();
//	}
//} // namespace Editor