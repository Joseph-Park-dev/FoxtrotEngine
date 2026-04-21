#pragma once
#include "Actor/Transform.h"

namespace Editor
{
	class EditorElement;
	class EditorTransform : public Core::Transform
	{
	public:
		void UpdateUI();

	public:
		Math::FTVector3& LocalPosition() const;
		Math::FTVector3& LocalScale() const;
		Math::FTVector3& LocalRotation() const;

		// World Transformation
		Math::FTVector3& WorldPosition() const;
		Math::FTVector3& WorldScale() const;
		Math::FTVector3& WorldRotation() const;

	public:
		EditorTransform(Editor::EditorElement* element);
	};
} // namespace Editor