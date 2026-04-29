#pragma once
#include "Component/Component.h"

namespace Editor
{
	template <typename COMPONENT>
	class EditorComponent
	{
	public:
		void UpdateUI()
		{
			COMPONENT::EditorUIUpdate();
		}
	};
} // namespace Editor