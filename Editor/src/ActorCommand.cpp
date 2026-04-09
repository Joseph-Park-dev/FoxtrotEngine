#include "ActorCommand.h"

#include "EventFunctions.h"
#include "EditorSceneManager.h"
#include "EditorElement.h"

void ActorAdditionCommand::Do()
{
	ActorCommand::Do();

	EditorScene* scene = EditorSceneManager::GetInstance()->GetEditorScene();
	// EditorElement* element = DBG_NEW EditorElement(GetVal());
}

void ActorAdditionCommand::Undo()
{
	ActorCommand::Undo();

	Core::Destroy(GetVal());
}

ActorAdditionCommand::ActorAdditionCommand(EditorElement* elem)
	: ActorCommand(elem)
{
}

void ActorCommand::Do()
{
	*mValue = mNextValue;
}

void ActorCommand::Undo()
{
	*mValue = mPrevValue;
}

EditorElement* ActorCommand::GetVal()
{
	return mValue;
}

void ActorCommand::SetNextVal(EditorElement* nextVal)
{
	mNextValue = *nextVal;
}

ActorCommand::ActorCommand(EditorElement* elem)
	: mPrevValue(*elem), mValue(elem), mNextValue(*elem)
{
}
