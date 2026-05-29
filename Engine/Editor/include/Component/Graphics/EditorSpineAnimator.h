#pragma once
#include "Component/IEditorComponent.h"
#include "Component/SpineAnimator.h"

#include <spine/spine.h>

#include "FTDS/Static/FTString.h"

namespace Editor
{
	class EditorSpineAnimator :
		public IEditorComponent<D3D11::SpineAnimator>
	{

	};
} // namespace Editor