#include "Actor/EditorTransform.h"

#include "CommandHistory.h"
#include "EditorElement.h"
#include "Actor/Actor.h"

namespace Editor
{
	/// @brief Exposes the position relative to the actor's parent.
	/// @return Borrowed access to the local position.
	/// @note Changes through the returned reference affect this object's stored state.
	Math::FTVector3& EditorTransform::LocalPosition() const
	{
		return const_cast<Math::FTVector3&>(GetLocalPosition());
	}

	/// @brief Exposes the scale relative to the actor's parent.
	/// @return Borrowed access to the local scale.
	/// @note Changes through the returned reference affect this object's stored state.
	Math::FTVector3& EditorTransform::LocalScale() const
	{
		return const_cast<Math::FTVector3&>(GetLocalScale());
	}

	/// @brief Exposes the rotation relative to the actor's parent.
	/// @return Borrowed access to the local rotation.
	/// @note Changes through the returned reference affect this object's stored state.
	Math::FTVector3& EditorTransform::LocalRotation() const
	{
		return const_cast<Math::FTVector3&>(GetLocalRotation());
	}

	/// @brief Exposes the actor's position in world coordinates.
	/// @return Borrowed access to the world position.
	/// @note Changes through the returned reference affect this object's stored state.
	Math::FTVector3& EditorTransform::WorldPosition() const
	{
		return const_cast<Math::FTVector3&>(GetWorldPosition());
	}

	/// @brief Exposes the actor's scale in world coordinates.
	/// @return Borrowed access to the world scale.
	/// @note Changes through the returned reference affect this object's stored state.
	Math::FTVector3& EditorTransform::WorldScale() const
	{
		return const_cast<Math::FTVector3&>(GetWorldScale());
	}

	/// @brief Exposes the actor's rotation in world coordinates.
	/// @return Borrowed access to the world rotation.
	/// @note Changes through the returned reference affect this object's stored state.
	Math::FTVector3& EditorTransform::WorldRotation() const
	{
		return const_cast<Math::FTVector3&>(GetWorldRotation());
	}

	/// @brief Initializes an editor-controlled transform for its actor.
	/// @param element Element to insert or edit.
	/// @note Initializes the :EditorTransform base or delegates to its constructor.
	EditorTransform::EditorTransform(Editor::EditorElement* element)
		: Core::Transform(element)
	{
	}

	/// @brief Builds the editor controls for inspecting and modifying this object's state.
	void EditorTransform::UpdateUI()
	{
		// The drag fields write the stored vector. The world matrix is rebuilt
		// from the local components inside the setters.
		CommandHistory* history = CommandHistory::GetInstance();
		if (!GetOwner()->GetParent())
		{
			history->UpdateVector3Value("World Position", WorldPosition());
			SetWorldPosition(GetWorldPosition());
			history->UpdateVector3Value("World Scale", WorldScale());
			SetWorldScale(GetWorldScale());
			history->UpdateVector3Value("World Rotation", WorldRotation());
			SetWorldRotation(GetWorldRotation());
		}
		else
		{
			history->UpdateVector3Value("Local Position", LocalPosition());
			SetLocalPosition(GetLocalPosition());
			history->UpdateVector3Value("Local Scale", LocalScale());
			SetLocalScale(GetLocalScale());
			history->UpdateVector3Value("Local Rotation", LocalRotation());
			SetLocalRotation(GetLocalRotation());
		}
	}
} // namespace Editor
