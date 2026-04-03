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
#include "ResourceSystem/CoreResource.h"

#include <iosfwd>

namespace Core
{
	class Actor;
	class FTCore;
	class Scene;

#ifdef FOXTROT_EDITOR
	class EditorScene;
	#include "EditorElement.h"
#endif // FOXTROT_EDITOR

	class FTPremade :
		public CoreResource
	{
		// public:
		//	virtual void SaveProperties(std::ofstream& ofs) override;
		//	virtual void LoadProperties(std::ifstream& ifs) override;

	public:
		FTPremade(FTResourceDef& resDef);
		~FTPremade();

	public:
		Actor* GetOrigin();
		bool   GetIsLoaded();

	private:
		// Member variable that holds the actual Actor Data.
		Actor* mOrigin;
		bool   mIsLoaded;

	private:
		void Load(const char* path);

#ifdef FOXTROT_EDITOR
	public:
		void Create(EditorElement* ele);
		void Save(EditorElement* ele);

		virtual void UpdateUI() override;

	public:
		virtual void AddRefCount() override;
		virtual void SubtractRefCount() override;

	private:
		EditorElement* mDummyForUI;
#endif // FOXTROT_EDITOR
	};

	namespace ChunkKey
	{
		namespace FTPremade
		{
			constexpr const char* FT_PREMADE = "FTPremade";

		}
	} // namespace ChunkKey
} // namespace Core