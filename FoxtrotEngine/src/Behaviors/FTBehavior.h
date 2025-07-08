#pragma once
#include "Components/Component.h"

#include "FileSystem/ChunkLoader.h"

#ifdef FOXTROT_EDITOR
#include "EditorChunkLoader.h"
#endif 

class Collider2D;

class FTBehavior :
    public Component
{
public:
    FTBehavior(Actor* owner, int updateOrder);
    virtual ~FTBehavior();

protected:
    virtual void OnCollisionEnter(Collider2D* other);
    virtual void OnCollisionStay(Collider2D* other);
    virtual void OnCollisionExit(Collider2D* other);
    friend class Collider2D;

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
    template <typename CUSTOM_BEHAVIOR>
    static void AddBehaviorToBuild(const char* key)
    {
        ChunkLoader::GetInstance()->GetComponentLoadMap().
            Insert(key, &FTBehavior::Load<CUSTOM_BEHAVIOR>);
    }
#endif
};

