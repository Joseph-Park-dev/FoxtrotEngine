#pragma once
#include "Component/IEditorComponent.h"
#include "Component/SpineAnimator.h"

#include <spine/spine.h>

#include "FTDS/Static/FTString.h"

namespace Editor
{
	class EditorSpineAnimator :
		public IEditorComponent,
		public D3D11::SpineAnimator
	{
	public:
		virtual void EditorUpdate(float deltaTime) override;
		virtual void EditorRender(Core::IRenderer* renderer, Core::ICamera* camInst) override;
		virtual void EditorUIUpdate() override;
	};
} // namespace Editor