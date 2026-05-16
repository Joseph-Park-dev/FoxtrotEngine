#pragma once
#include "Component/IComponent.h"

namespace Core
{
	class FTBehavior :
		public IComponent
	{
	public:
		FTBehavior(Actor* owner, int updateOrder = DefaultVal::UPDATE_ORDER);
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