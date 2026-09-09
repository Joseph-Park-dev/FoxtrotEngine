#include "EditorEventFunctions.h"

#include "EditorSceneManager.h"
#include "EditorScene.h"
#include "EditorElement.h"

#include "Manager/ResourceManager.h"
#include "Manager/EventManager.h"
#include "EventSystem/EventType.h"

namespace Editor
{
	/// @brief Creates an actor instance and schedules it for the requested scene or group.
	/// @param actor Actor participating in this operation.
	/// @param actorGroup Actor group used for scene classification.
	/// @param pos Position or zero-based insertion index.
	/// @return Created instantiate instance or resource.
	EditorElement* Editor::Instantiate(Core::IActor* actor, Core::ActorGroup actorGroup, Math::FTVector3 pos)
	{
		Core::FTEvent addedEvent = {};
		addedEvent.incident		 = Core::EVENT_TYPE::CREATE_ACTOR;
		actor->SetActorGroup(actorGroup);

		EditorScene*		   scene = EditorSceneManager::GetInstance()->GetEditorScene();
		Editor::EditorElement* elem	 = DBG_NEW Editor::EditorElement(actor, Common::ChunkKey::ID::CLONE);
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

	/// @brief Releases the managed instance or schedules the specified actor for destruction.
	/// @param actor Actor participating in this operation.
	void Editor::Destroy(EditorElement* actor)
	{
		Core::FTEvent addedEvent = {};
		addedEvent.incident		 = Core::EVENT_TYPE::DESTROY_ACTOR;
		// addedEvent.eventData.push_back(actor);
		Core::EventManager::GetInstance()->AddEvent(addedEvent);
	}
} // namespace Editor
