#pragma once
#include "Component/IComponent.h"

namespace Core
{
	class FTBehavior :
		public IComponent
	{
	public:
		/// @brief Initializes behavior state associated with its actor.
		/// @param owner Actor or object associated with the new instance.
		/// @param updateOrder Order used when dispatching component updates.
		FTBehavior(Actor* owner, int updateOrder = DefaultVal::UPDATE_ORDER);
		/// @brief Completes destruction through the object's inheritance hierarchy.
		virtual ~FTBehavior();

	public:
#ifdef FOXTROT_EDITOR
		// template <typename CUSTOM_BEHAVIOR>
		// static void AddBehaviorToEditor(const char* key)
		//{
		//	EditorChunkLoader::GetInstance()->GetCompCreateMap().insert(std::make_pair(key, &FTBehavior::Create<CUSTOM_BEHAVIOR>));

		//	ChunkLoader::GetInstance()->GetComponentLoadMap().Insert(key, &FTBehavior::Load<CUSTOM_BEHAVIOR>);
		//}

#else
		// template <typename CUSTOM_BEHAVIOR>
		// static void AddBehaviorToBuild(const char* key)
		//{
		//     ChunkLoader::GetInstance()->GetComponentLoadMap().
		//         Insert(key, &FTBehavior::Load<CUSTOM_BEHAVIOR>);
		// }
#endif
	};
} // namespace Core
