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

public:
    bool GetIsLoaded();

public:
    Actor* GetOrigin();

private:
    Actor*  mOrigin;
    size_t  mCount;
    bool    mIsLoaded;

public:
    virtual void SaveProperties(std::ofstream& ofs, UINT key) override;
    virtual UINT LoadProperties(std::ifstream& ifs) override;

#ifdef FOXTROT_EDITOR
public:
            void Create (EditorElement* ele);
            void Save   (EditorElement* ele);

    virtual void UpdateUI ()        override;
#endif // FOXTROT_EDITOR
};

