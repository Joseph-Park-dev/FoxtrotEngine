#include "Actor/EditorTransform.h"

#include "CommandHistory.h"
#include "EditorElement.h"
#include "Actor/Actor.h"

namespace Editor
{
	Math::FTVector3& EditorTransform::LocalPosition() const
	{
		return const_cast<Math::FTVector3&>(GetLocalPosition());
	}

	Math::FTVector3& EditorTransform::LocalScale() const
	{
		return const_cast<Math::FTVector3&>(GetLocalScale());
	}

	Math::FTVector3& EditorTransform::LocalRotation() const
	{
		return const_cast<Math::FTVector3&>(GetLocalRotation());
	}

	Math::FTVector3& EditorTransform::WorldPosition() const
	{
		return const_cast<Math::FTVector3&>(GetWorldPosition());
	}

	Math::FTVector3& EditorTransform::WorldScale() const
	{
		return const_cast<Math::FTVector3&>(GetWorldScale());
	}

	Math::FTVector3& EditorTransform::WorldRotation() const
	{
		return const_cast<Math::FTVector3&>(GetWorldRotation());
	}

	EditorTransform::EditorTransform(Editor::EditorElement* element)
		: Core::Transform(element)
	{
	}

	void EditorTransform::UpdateUI()
	{
		if (!GetOwner()->GetParent()) // The owner does not have parent Actor.
		{
			CommandHistory::GetInstance()->UpdateVector3Value("World Position", WorldPosition());
			CommandHistory::GetInstance()->UpdateVector3Value("World Scale", WorldScale());
			CommandHistory::GetInstance()->UpdateVector3Value("World Rotation", WorldRotation());
		}
		else
		{
			CommandHistory::GetInstance()->UpdateVector3Value("Local Position", LocalPosition());
			CommandHistory::GetInstance()->UpdateVector3Value("Local Scale", LocalScale());
			CommandHistory::GetInstance()->UpdateVector3Value("Local Rotation", LocalRotation());
		}
	}
} // namespace Editor