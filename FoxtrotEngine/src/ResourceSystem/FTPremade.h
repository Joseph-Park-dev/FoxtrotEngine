#pragma once
#include "ResourceSystem/FTResource.h"

class Actor;
class FTCore;
class Scene;
#ifdef FOXTROT_EDITOR
class EditorElement;
class EditorScene;
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

public:
    void AddToScene(Scene* scene, FTCore* ftCoreInst);
    bool GetIsLoaded();

public:
    Actor* GetOrigin();

private:
    // Member variable that holds the actual Actor Data.
    Actor*  mOrigin;
    bool    mIsLoaded;

public:
    virtual void SaveProperties(std::ofstream& ofs, UINT key) override;
    virtual UINT LoadProperties(std::ifstream& ifs) override;

#ifdef FOXTROT_EDITOR
public:
            void Create (EditorElement* ele);
            void Save   (EditorElement* ele);

    virtual void UpdateUI ()        override;
            EditorElement* AddToScene(EditorScene* scene);
#endif // FOXTROT_EDITOR
};

