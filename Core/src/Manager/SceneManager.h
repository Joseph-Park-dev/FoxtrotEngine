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
#include "Entity/Entity.h"

#include <iosfwd>

#include "SingletonMacro.h"
#include "FTDS/Static/FTString.h"

namespace Core
{
	class Scene;
	class FoxtrotRenderer;
	class FTInputDevice;
	namespace FTDS
	{
		template <typename TYPE>
		class DynamicArray;
	} // namespace FTDS

#ifndef FOXTROT_EDITOR // If it's not on Editor.
	#define FIND_ACTOR_FILTER(name, filter) SceneManager::GetInstance()->GetCurrentScene()->FindActor(name, filter)
	#define FIND_ACTOR(id) SceneManager::GetInstance()->GetCurrentScene()->FindActor(id)
#endif // FOXTROT_EDITOR

	class SceneManager :
		public Core::Entity
	{
		SINGLETON_PROTECTED(SceneManager)

	public:
		void		   SwitchScene(size_t index);
		virtual Scene* GetCurrentScene();

	public:
		void SetChunkListPath(FTDS::String&& path);

		FTDS::DynamicArray<FTDS::String*>*& ChunkList();

	public:
		virtual void Initialize(Scene* scene);
		virtual void ProcessEvent();

	public:
		void SaveSceneList(std::ofstream& ofs);

	protected:
		void RegisterMemberFuncs() override;

	private:
		FTDS::String					   mChunkListPath;
		Scene*							   mCurrentScene;
		FTDS::DynamicArray<FTDS::String*>* mChunkList; // List, and order of .chunks
	};
} // namespace Core