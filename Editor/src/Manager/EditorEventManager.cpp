#include "EditorEventManager.h"

#include "EditorEventFunctions.h"
#include "EditorSceneManager.h"
#include "EditorScene.h"
#include "EditorElement.h"
#include "EventType.h"

namespace Editor
{
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

	EditorEventManager::EditorEventManager()
		: Core::EventManager()
	{
	}

	EditorEventManager::~EditorEventManager()
	{
	}
} // namespace Editor