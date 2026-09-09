#include "Manager/EditorEventManager.h"

#include "EditorEventFunctions.h"
#include "EditorSceneManager.h"
#include "EditorScene.h"
#include "EditorElement.h"
#include "EventSystem/EventType.h"

namespace Editor
{
	/// @brief Applies the requested event or command to engine state.
	/// @param executedEvent Event whose operation is applied.
	void EditorEventManager::Execute(const Core::FTEvent& executedEvent)
	{
		switch (executedEvent.incident)
		{
			case Core::EVENT_TYPE::CREATE_ACTOR:
			{
				Editor::EditorElement* created = static_cast<Editor::EditorElement*>(executedEvent.eventData);
				Editor::EditorScene*   scene   = Editor::EditorSceneManager::GetInstance()->GetEditorScene();
				scene->AddEditorElement(created);
			}
			break;

			case Core::EVENT_TYPE::DESTROY_ACTOR:
			{
				Editor::EditorElement* element = static_cast<Editor::EditorElement*>(executedEvent.eventData);
				Editor::EditorScene*   scene   = Editor::EditorSceneManager::GetInstance()->GetEditorScene();
				scene->RemoveActor(element);
			}
			break;
			case Core::EVENT_TYPE::SWITCH_SCENE:
			{
				EditorSceneManager::GetInstance()->SwitchScene(*static_cast<size_t*>(executedEvent.eventData));
			}
			break;
		}
	}

	/// @brief Initializes the pending editor-event collection.
	/// @note Initializes the :EditorEventManager base or delegates to its constructor.
	EditorEventManager::EditorEventManager()
		: Core::EventManager()
	{
	}

	/// @brief Completes destruction through the object's inheritance hierarchy.
	EditorEventManager::~EditorEventManager()
	{
	}
} // namespace Editor
