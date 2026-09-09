#include "ActorCommand.h"

#include "EditorSceneManager.h"
#include "EditorElement.h"

namespace Editor
{
	/// @brief Applies the command's change to the target object.
	void ActorAdditionCommand::Do()
	{
		ActorCommand::Do();

		// EditorScene* scene = EditorSceneManager::GetInstance()->GetCurrentScene();
		//  EditorElement* element = DBG_NEW EditorElement(GetVal());
	}

	/// @brief Restores the target state saved before the command was applied.
	void ActorAdditionCommand::Undo()
	{
		ActorCommand::Undo();

		// Core::Destroy(GetVal());
	}

	/// @brief Initializes an undoable actor insertion.
	/// @param elem Editor element targeted by the operation.
	/// @note Initializes the :ActorAdditionCommand base or delegates to its constructor.
	ActorAdditionCommand::ActorAdditionCommand(EditorElement* elem)
		: ActorCommand(elem)
	{
	}

	/// @brief Applies the command's change to the target object.
	void ActorCommand::Do()
	{
		mValue = mNextValue;
	}

	/// @brief Restores the target state saved before the command was applied.
	void ActorCommand::Undo()
	{
		mValue = mPrevValue;
	}

	/// @brief Returns the val used by this actor command.
	/// @return Borrowed access to the val.
	EditorElement* ActorCommand::GetVal()
	{
		return mValue;
	}

	/// @brief Updates the next val used by subsequent operations.
	/// @param nextVal Replacement next val.
	void ActorCommand::SetNextVal(EditorElement* nextVal)
	{
		mNextValue = nextVal;
	}

	/// @brief Initializes the actor target used by undoable editor operations.
	/// @param elem Editor element targeted by the operation.
	/// @note Initializes the :ActorCommand base or delegates to its constructor.
	ActorCommand::ActorCommand(EditorElement* elem)
		: mPrevValue(elem), mValue(elem), mNextValue(elem)
	{
	}
} // namespace Editor
