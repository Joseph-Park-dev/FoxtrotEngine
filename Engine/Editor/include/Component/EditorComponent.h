#pragma once
#include "Component/IComponent.h"

#include "Renderer/D3D11Renderer.h"

namespace Editor
{
	template <typename DERIVED>
	class EditorComponent : public Core::Component
	{
	public:
		virtual void EditorUIUpdate()
		{
			static_cast<DERIVED*>(this)->EditorUIUpdate();
		}

	public:
		virtual void EditorRender(D3D11::D3D11Renderer* renderer)
		{
			static_cast<DERIVED*>(this)->EditorRender(renderer);
		}
	};
} // namespace Editor