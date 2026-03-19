#pragma once
#include "Component/Component.h"

class Collider2D;

class FTBehavior :
    public Component
{
public:
	FTBehavior(Plugin* plugin, Actor* owner, int updateOrder = DefaultVal::UPDATE_ORDER);
    virtual ~FTBehavior();

public:
#ifdef FOXTROT_EDITOR
    template <typename CUSTOM_BEHAVIOR>
    static void AddBehaviorToEditor(const char* key)
    {
        EditorChunkLoader::GetInstance()->GetCompCreateMap().
            insert(std::make_pair(key, &FTBehavior::Create<CUSTOM_BEHAVIOR>));

        ChunkLoader::GetInstance()->GetComponentLoadMap().
            Insert(key, &FTBehavior::Load<CUSTOM_BEHAVIOR>);
    }

#else
    //template <typename CUSTOM_BEHAVIOR>
    //static void AddBehaviorToBuild(const char* key)
    //{
    //    ChunkLoader::GetInstance()->GetComponentLoadMap().
    //        Insert(key, &FTBehavior::Load<CUSTOM_BEHAVIOR>);
    //}
#endif
};

