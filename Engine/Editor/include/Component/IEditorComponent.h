#pragma once
#include "Component/IComponent.h"

namespace Core
{
	class IRenderer;
	class ICamera;
} // namespace Core

namespace Editor
{
	class IEditorComponent :
		public Core::IComponent
	{
	public:
		virtual void EditorUpdate(float deltaTime)									 = 0;
		virtual void EditorRender(Core::IRenderer* renderer, Core::ICamera* camInst) = 0;
		virtual void EditorUIUpdate()												 = 0;
	};
} // namespace Editor