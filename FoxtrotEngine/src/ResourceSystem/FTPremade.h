#pragma once
#include "ResourceSystem/FTResource.h"

#ifdef FOXTROT_EDITOR
#include "EditorElement.h"
#endif // FOXTROT_EDITOR

#define FTPremade_FORMAT_SUPPORTED ".premade"

class FTPremade :
    public FTResource
{
public:
    FTPremade();
    ~FTPremade() override;

public:
    void Load();
    void AddToScene(Scene* scene, FTCore* coreInst);

private:
    Actor*  mOrigin;
    size_t  mCount;

#ifdef FOXTROT_EDITOR
public:
            void Create (EditorElement* ele);
            void Save   (EditorElement* ele);

    virtual void UpdateUI ()        override;
#endif // FOXTROT_EDITOR
};

