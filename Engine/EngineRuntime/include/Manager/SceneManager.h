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
#include "Plugin/CoreExports.h"
#include "Entity/Entity.h"

#include <iosfwd>

#include "Utility/SingletonMacro.h"
#include "FTDS/Static/FTString.h"

#include "Plugin/CoreExports.h"

namespace Common
{
	class IActor;
	namespace FTDS
	{
		template <typename TYPE>
		class DynamicArray;
	} // namespace FTDS
} // namespace Common

namespace Core
{
	class Scene;

#ifndef FOXTROT_EDITOR // If it's not on Editor.
	#define FIND_ACTOR_FILTER(name, filter) SceneManager::GetInstance()->GetCurrentScene()->FindActor(name, filter)
	#define FIND_ACTOR(id) SceneManager::GetInstance()->GetCurrentScene()->FindActor(id)
#endif // FOXTROT_EDITOR

	class CORE_API SceneManager
	{
		SINGLETON_PROTECTED(SceneManager)

	public:
		void		   SwitchScene(size_t index);
		virtual Scene* GetCurrentScene();

	public:
		void SetChunkListPath(Common::FTDS::String&& path);

		Common::FTDS::DynamicArray<Common::FTDS::String*>*& ChunkList();

	public:
		virtual void Initialize(Scene* scene);
		virtual void ProcessEvent();

	public:
		void SaveSceneList(std::ofstream& ofs);

	private:
		Common::FTDS::String							   mChunkListPath;
		Scene*											   mCurrentScene;
		Common::FTDS::DynamicArray<Common::FTDS::String*>* mChunkList; // List, and order of .chunks
	};

	extern "C"
	{
		CORE_API Common::IActor* FindActor(const char* name, Common::IActor* filter);
	}
} // namespace Core