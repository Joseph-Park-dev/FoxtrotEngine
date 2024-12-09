#pragma once
#include "Core/SingletonMacro.h"
#include "Scenes/SceneType.h"

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
			//void LoadSceneList();

private:
	std::string				 mChunkListPath;
	
	Scene*					 mCurrentScene;
	std::vector<std::string> mChunkList; // List, and order of .chunks
};