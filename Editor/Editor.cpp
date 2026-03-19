// Editor.cpp : Defines the exported functions for the DLL.
//

#include "pch.h"
#include "framework.h"
#include "Editor.h"


// This is an example of an exported variable
EDITOR_API int nEditor=0;

// This is an example of an exported function.
EDITOR_API int fnEditor(void)
{
    return 0;
}

Editor::Editor()
{
}

void Editor::Initialize()
{
}

void Editor::Setup()
{
}

void Editor::ProcessInput(FTInputDevice* inputDevice)
{
}

void Editor::Update(float deltaTime)
{
}

void Editor::LateUpdate(float deltaTime)
{
}

void Editor::Render(FoxtrotRenderer* renderer)
{
}

void Editor::Clear()
{
}
