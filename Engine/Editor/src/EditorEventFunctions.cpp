#include "EditorEventFunctions.h"

#include <EditorSceneManager.h>
#include <EditorScene.h>
#include <EditorElement.h>

#include "Manager/ResourceManager.h"
#include "Manager/EventManager.h"
#include "EventType.h"

namespace Editor
{
	EditorElement* Editor::Instantiate(Core::Actor* actor, Core::ActorGroup actorGroup, Math::FTVector3 pos)
	{
		Core::FTEvent addedEvent = {};
		addedEvent.incident		 = Core::EVENT_TYPE::CREATE_ACTOR;
		actor->SetActorGroup(actorGroup);

		EditorScene*		   scene = EditorSceneManager::GetInstance()->GetEditorScene();
		Editor::EditorElement* elem	 = DBG_NEW Editor::EditorElement(actor, Core::ChunkKey::ID::CLONE);
		elem->Initialize();

		if (elem)
		{
			addedEvent.eventData = elem;
			Core::EventManager::GetInstance()->AddEvent(addedEvent);
			return elem;
		}
		else
		{
			printf("ERROR : Instantiate() -> Premade not loaded, %s\n", actor->GetName().C_Str());
			return nullptr;
		}
	}

	EditorElement* Editor::Instantiate(const char* premadeName)
	{
		Core::FTEvent addedEvent = {};
		addedEvent.incident		 = Core::EVENT_TYPE::CREATE_ACTOR;
		Core::FTPremade* premade = Core::GET_RES(Core::FTPremade, premadeName);
		Core::Actor* origin = premade->GetOrigin();

		EditorScene*		   scene		 = EditorSceneManager::GetInstance()->GetEditorScene();
		Editor::EditorElement* editorElement = DBG_NEW Editor::EditorElement(origin, Core::ChunkKey::ID::CLONE);
		editorElement->Initialize();

		if (editorElement)
		{
			addedEvent.eventData = editorElement;
			Core::EventManager::GetInstance()->AddEvent(addedEvent);
			return editorElement;
		}
		else
		{
			printf("ERROR : Instantiate() -> Premade not loaded, %s\n", premadeName);
			return nullptr;
		}
	}

	void Editor::Destroy(EditorElement* actor)
	{
		Core::FTEvent addedEvent	= {};
		addedEvent.incident = Core::EVENT_TYPE::DESTROY_ACTOR;
		// addedEvent.eventData.push_back(actor);
		Core::EventManager::GetInstance()->AddEvent(addedEvent);
	}
} // namespace Editor