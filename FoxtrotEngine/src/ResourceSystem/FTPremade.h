// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Pre-defined Actors that is saved as a file.
/// This can be instantiated during the game loop through the function
/// "Instantiate".
/// Similar role as "Prefab"s in Unity game engine.
/// </summary>

#pragma once
#include "ResourceSystem/FTResource.h"

class Actor;
class FTCore;
class Scene;

#ifdef FOXTROT_EDITOR
class EditorScene;
#include "EditorElement.h"
#endif // FOXTROT_EDITOR

class FTPremade :
    public FTResource
{
public:
    FTPremade();
    ~FTPremade() override;

public:
    void    Load();

public:
    Actor*  GetOrigin();
    bool    GetIsLoaded();

private:
    // Member variable that holds the actual Actor Data.
    Actor*  mOrigin;
    bool    mIsLoaded;

public:
    virtual void SaveProperties(std::ofstream& ofs, UINT key) override;
    virtual UINT LoadProperties(std::ifstream& ifs) override;

#ifdef FOXTROT_EDITOR
public:
            void           Create (EditorElement* ele);
            void           Save   (EditorElement* ele);

    virtual void           UpdateUI ()        override;

private:
    EditorElement* mDummyForUI;
#endif // FOXTROT_EDITOR
};

