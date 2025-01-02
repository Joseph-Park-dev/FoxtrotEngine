// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// A singleton manager that holds a Scene instance, switches a Scene, 
/// and runs the Scene as a part of the game loop.
/// </summary>

#pragma once
#include "Core/SingletonMacro.h"

class Scene;
class FoxtrotRenderer;
class KeyInputManager;

class SceneManager
{
	SINGLETON(SceneManager)

public:
			void	SwitchScene(size_t index);
			Scene*	GetCurrentScene();

public:
	std::vector<std::string>& GetChunkList();
	void					  SetChunkListPath(std::string&& path);

public:
	virtual void Initialize		();
	virtual void ProcessInput	(KeyInputManager* keyInputManager);
	virtual void Update			(float deltaTime);
	virtual void Lateupdate		(float deltaTime);
	virtual void Render			(FoxtrotRenderer* renderer);
	virtual void ProcessEvent	();

public:
			void SaveSceneList(std::ofstream& ofs);

private:
	std::string				 mChunkListPath;
	
	Scene*					 mCurrentScene;
	std::vector<std::string> mChunkList; // List, and order of .chunks
};